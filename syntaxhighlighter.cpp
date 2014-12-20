#include "syntaxhighlighter.h"


/**************************************************************************************
 *  Construct a SyntaxHighligher Object. This involves first connect()-ing            *
 *  the MainWindow::syntaxChanged() signal with the SyntaxHighlighter::changeSyntax() *
 *  slot, as well as constructing a QStringList of syntax highlighting rules          *
 *************************************************************************************/
SyntaxHighlighter::SyntaxHighlighter(QTextDocument* textDocument, MainWindow *mainWindow) :
    QSyntaxHighlighter(textDocument), m_document(textDocument), m_MainWindow(mainWindow)
{

    /**************************************************************************
    * Hook our SyntaxHighlighter up to the MainWindow syntaxChanged signal    *
    * The MainWindow emits this signal when the user selects the              *
    * File->New Template option from the MenuBar. The file must be of *.tax   *
    * format. The signal passes the filename to the signal and the            *
    * SyntaxHighlighter takes care of the rest.                               *
    **************************************************************************/
    connect(m_MainWindow, SIGNAL(syntaxChanged(QString)), this, SLOT(changeSyntax(QString)));



    /************************************************************************
    * We will only create one Rule struct on the stack and reuse this same  *
    * struct for every rule we add to our QStringList<Rule> rulesList       *
    ************************************************************************/
    Rule rule;
    rule.format.setFontWeight(QFont::Bold);
    rule.format.setForeground(Qt::darkBlue);



    /****************************************************************************
     * Here we begin the construction of our QStringList<Rule> rulesList.       *
     * We start by putting in Regex's for all the C++ keyworks. These will all  *
     * share the same formatting.                                               *
     ***************************************************************************/
    QStringList keywordList;
    keywordList  << "\\bchar\\b"     << "\\bclass\\b"     << "\\bconst\\b"
                 << "\\bdouble\\b"   << "\\benum\\b"      << "\\bexplicit\\b"
                 << "\\bfriend\\b"   << "\\binline\\b"    << "\\bint\\b"
                 << "\\blong\\b"     << "\\bnamespace\\b" << "\\boperator\\b"
                 << "\\bprivate\\b"  << "\\bprotected\\b" << "\\bpublic\\b"
                 << "\\bshort\\b"    << "\\bsignals\\b"   << "\\bsigned\\b"
                 << "\\bslots\\b"    << "\\bstatic\\b"    << "\\bstruct\\b"
                 << "\\btemplate\\b" << "\\btypedef\\b"   << "\\btypename\\b"
                 << "\\bunion\\b"    << "\\bunsigned\\b"  << "\\bvirtual\\b"
                 << "\\bvoid\\b"     << "\\bvolatile\\b";


    /*******************************************************************************
     * Iterate through the list we just constructed and add rules for each pattern *
     ******************************************************************************/
    foreach(const QString& pattern, keywordList) {
        rule.regEx = QRegExp(pattern);
        rulesList.append(rule);
    }



    /* **************************************************************************
     * Now we can move on to other C++ Regex's and append them to the rulesList *
     * *************************************************************************/
    // C++ Class Rule
    rule.regEx = QRegExp( "\\b[A-Z][_A-Za-z0-9]*\\b" );
    rule.format.setFontWeight(QFont::Bold);
    rule.format.setForeground(Qt::darkRed);
    rulesList.append(rule);

    // C++ Quotes Rule
    rule.regEx = QRegExp( "\\b\".*\"\\b" );
    rule.format.setForeground(Qt::blue);
    rulesList.append(rule);
    rule.regEx = QRegExp( "\\b\'.*\'\\b" );
    rulesList.append( rule );

    // C++ Functions Rule
    rule.regEx = QRegExp( "\\b[A-Za-z0-9_]+(?=\\()" );
    rule.format.setForeground(QBrush(QColor(175,207,219)));
    rulesList.append(rule);

    // C++ Comments Rule
    rule.regEx = QRegExp( "//[^\n]*" );
    rule.format.setFontWeight(QFont::Normal);
    rule.format.setForeground(Qt::lightGray);;
    rulesList.append(rule);
}



/************************************************************************************
*  The highlightBlock method is called for every line of the document every time it *
*  is updated. We have to traverse the line for every rule in our rulesList and     *
*  check for a match. Where there is a match, we update the text format of the      *
*  match to the format specified in the rule.format field.                          *
************************************************************************************/
void SyntaxHighlighter::highlightBlock(const QString& text)
{
    if(rulesList.isEmpty()) {
        QMessageBox::information( m_MainWindow, QString("Uh oh"), QString("The rulesList is empty"), QMessageBox::Ok );
        return;
    }
    foreach(const Rule rule, rulesList) {
        int startIndex = rule.regEx.indexIn(text);                       // Returns 0 or positive number if there is a match
        int length;
        while( startIndex >= 0 ) {
            length = rule.regEx.matchedLength();
            setFormat(startIndex, length, rule.format);
            startIndex = rule.regEx.indexIn(text, startIndex+length);    // Move on to the next match AFTER the first match. If another match isn't found, startIndex will be < 0 and loop will exit.
        }
    }
}



/******************************************************************************
 * Very simply, this will delete itself. Qt takes care of deleting everything *
 * associated with this object.                                               *
 *****************************************************************************/
SyntaxHighlighter::~SyntaxHighlighter()
{
    delete this;
}



/*********************************************************************
 * Custom slot to parse new .tax file and create a new rulesList     *
 * This method is called when the "File->New Template" menu item is  *
 * selected from the MainWindow and it is passed the file path       *
 * of the .tax config file selected from a QFileDialog               *
 ********************************************************************/
void SyntaxHighlighter::changeSyntax(QString fileName) {

    // We want to save everything in a temporary rulesList because if we overrite our main rulesList and there is an error (due to an incorrect XML file for example), then we will have to restart the application
    QVector<Rule> tempRulesList;

    // Once again, we will only use one Rule struct and reuse it so we don't create a ton of these things
    Rule tempRule;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::critical(m_MainWindow, QString("Error"), QString("Could not open syntax template %1").arg(fileName), QMessageBox::Ok);

    QByteArray   jsonByteArray = file.readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonByteArray);

    /******************************************************************************************/
    /* At this point, we have a json document of user defined rules. Need to extract data and */
    /* put it in to the QVector<SyntaxHighlighter::Rule> rulesList                            */
    /******************************************************************************************/

    tempRulesList = getRules(jsonDocument);


}




/**************************************************************************
 * This method is handed the QXmlStreamReader and extracts the font       *
 * format information. It does not modify the QXmlStreamReader in any way *
 * and leaves the cursor where it is. It is used to populate the          *
 * Rule::format field of the rule struct                                  *
 *************************************************************************/
QTextCharFormat SyntaxHighlighter::getFormat(const QXmlStreamReader& xsr) {
    if(xsr.name() != "rule")
        throw(QXmlStreamReader::UnexpectedElementError);

    QTextCharFormat result;

    // Extract Color Attribute
    if(xsr.attributes().hasAttribute("color")) {
        QString color = xsr.attributes().value("color").toString();
        if(color == "blue")
            result.setForeground(Qt::blue);
        else if(color == "dark-blue")
            result.setForeground(Qt::darkBlue);
        else if(color == "gray")
            result.setForeground(Qt::gray);
        else if(color == "light-gray")
            result.setForeground(Qt::lightGray);
        else if(color == "red")
            result.setForeground(Qt::red);
        else if(color == "green")
            result.setForeground(Qt::green);
        else
            QMessageBox::critical(m_MainWindow, QString("Attribute Error"), QString("Color attribute was not an expected value"), QMessageBox::Ok);
    }

    // Extract Font Attribute
    if(xsr.attributes().hasAttribute("style")) {
        QStringRef style = xsr.attributes().value("style");
        if(style == "bold")
            result.setFontWeight(QFont::Bold);
        else if(style == "italic")
            result.setFontItalic(true);
        else if(style == "normal")
            result.setFontWeight(QFont::Normal);
        else
            QMessageBox::critical(m_MainWindow, QString("Style Error"), QString("Style attribute was not an expected value"), QMessageBox::Ok);
    }

    return result;
}



/*****************************************************************
 * A (not very sophisticated) string to integer conversion       *
 * utility function. This is so that we can use a C++ switch     *
 * statement to evaluate tags in the XML file. Needs future      *
 * improvement.                                                  *
 ****************************************************************/
int SyntaxHighlighter::str2int(QString str) {
    int i = 0;
    foreach(QChar a, str) {
        i += a.unicode();
    }
    return i;
}

/****************************************************************************
* I wanted to keep this code to get the syntax rules seperated from the     *
* rest of the code to keep it uncluttered. This is a private method called  *
* from within the changeSyntax() method which accepts the JSON config file  *
* and returns a vector of the new rules.                                    *
****************************************************************************/

QVector<SyntaxHighlighter::Rule>& SyntaxHighlighter::getRules(const QJsonDocument& jsonDoc) {
    QJsonObject jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();

    Rule rule;
    QVector<Rule> rulesVector;
    rulesVector.append(rule);
    return rulesVector;
}
