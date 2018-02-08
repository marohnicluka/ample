#include "commandindexdialog.h"
#include "ui_commandindexdialog.h"
#include <QDebug>
#include <QVariant>
#include <QFont>
#include <QDesktopServices>

CommandIndexDialog::CommandIndexDialog(CommandIndex *ci, QWidget *parent) :
    QDialog(parent),
    commandIndex(ci),
    ui(new Ui::CommandIndexDialog)
{
    ui->setupUi(this);
    populateIndex();
    ui->commandTreeWidget->sortItems(0, Qt::AscendingOrder);
    ui->commandHelpBrowser->setFont(QFont("FreeSerif", 12));
    ui->commandHelpBrowser->setOpenLinks(false);
    //ui->commandHelpBrowser->setOpenExternalLinks(true);
}

CommandIndexDialog::~CommandIndexDialog()
{
    delete ui;
}

QTreeWidgetItem *CommandIndexDialog::findChapter(const QString &name)
{
    QTreeWidgetItem *item;
    foreach (item, chapters)
    {
        if (item->text(0) == name)
            return item;
    }
    item = new QTreeWidgetItem(ui->commandTreeWidget);
    item->setText(0, name);
    chapters.append(item);
    return item;
}

QTreeWidgetItem *CommandIndexDialog::findSection(QTreeWidgetItem *chapter, const QString &name)
{
    QTreeWidgetItem *item;
    int n = chapter->childCount();
    for (int i = 0; i < n; ++i)
    {
        item = chapter->child(i);
        if (item->childCount() > 0 && item->text(0) == name)
            return item;
    }
    item = new QTreeWidgetItem(chapter);
    item->setText(0, name);
    return item;
}

void CommandIndexDialog::populateIndex()
{
    int n = commandIndex->commandCount(), count = 0;
    for (int i = 0; i < n; ++i)
    {
        const Command &command = commandIndex->commandAt(i);
        QString category;
        foreach (category, command.categories)
        {
            QStringList sections = category.split(":");
            QString chapterName = sections.front(), sectionName, name;
            sections.pop_front();
            QTreeWidgetItem *chapter = findChapter(chapterName);
            QTreeWidgetItem *section = chapter;
            foreach (sectionName, sections)
                section = findSection(section, sectionName);
            QTreeWidgetItem *item;
            foreach (name, command.names)
            {
                item = new QTreeWidgetItem(section);
                item->setText(0, name);
                item->setData(0, Qt::UserRole, QVariant(i));
                count++;
            }
        }
    }
    qInfo() << tr("%1 command index entries loaded.").arg(count);
}

void CommandIndexDialog::appendSectionTitle(QString &text, const QString &title)
{
    text += QString("<h4 style=\"font-family:freesans\">%1</h4>").arg(title);
}

QString CommandIndexDialog::formatReference(const Reference &reference)
{
    QString source;
    if (reference.source == "em")
        source = "<i>Encyclopedia of Mathematics</i>";
    else if (reference.source == "wiki")
    {
        if (reference.language == "" || reference.language == "en")
            source = "<i>Wikipedia</i>, the free encyclopedia";
        else if (reference.language == "fr")
            source = "<i>Wikipédia</i>, l'encyclopédie libre";
        else if (reference.language == "es")
            source = "<i>Wikipedia</i>, la enciclopedia libre";
        else if (reference.language == "de")
            source = "<i>Wikipedia</i>, die freie Enzyklopädie";
        else if (reference.language == "el")
            source = "<i>Βικιπαίδεια</i>, την ελεύθερη εγκυκλοπαίδεια";
        else if (reference.language == "zh")
            source = "維基百科，自由的百科全書";
        else if (reference.language == "hr")
            source = "<i>Wikipedija</i>, slobodna enciklopedija";
    }
    return source.length() > 0 ?
                (reference.language == "zh" ?
                     QString("<p>%1。%2。</p>").arg(reference.title).arg(source) :
                     QString("<p>%1. %2.</p>").arg(reference.title).arg(source)) :
                QString("<p>%1</p>").arg(reference.title);
}

QString CommandIndexDialog::formatInputSyntax(const InputSyntax &inputSyntax)
{
    QStringList returnValueTypes;
    ReturnType returnType;
    foreach (returnType, inputSyntax.returnTypes) {
        QString returnTypeString = CommandIndex::paramTypeToString(returnType.type, 0);
        QString returnSubtypeString = CommandIndex::paramTypeToString(returnType.subtype, 1);
        if (returnSubtypeString.length() > 0)
            returnTypeString += QString(" %1").arg(returnSubtypeString);
        returnValueTypes.append(returnTypeString);
    }
    QString formattedInputSyntax = QString("<p>%1 (%2 %3)").arg(
                tr("Input arguments")).arg(tr("return type:")).arg(returnValueTypes.join(QString(" %1 ").arg(tr("or"))));
    CommandParameter param;
    formattedInputSyntax += "<ul>";
    foreach (param, inputSyntax.parameters)
        formattedInputSyntax += QString("<li>%1</li>").arg(param.format());
    formattedInputSyntax += "</ul>";

    formattedInputSyntax += "</p>";
    return formattedInputSyntax;
}

void CommandIndexDialog::on_commandTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current->childCount() > 0)
        return;
    int index = current->data(0, Qt::UserRole).toInt();
    const Command &command = commandIndex->commandAt(index);
    QString text("<html><style>rd{text-decoration:overline;}</style><body>"), typeString;
    switch (command.type)
    {
    case CmdTypeCommand:
        typeString = tr("Command");
        break;
    case CmdTypeConstant:
        typeString = tr("Constant");
        break;
    case CmdTypeKeyword:
        typeString = tr("Keyword");
        break;
    case CmdTypeOperator:
        typeString = tr("Operator");
        break;
    case CmdTypeOption:
        typeString = tr("Option");
        break;
    case CmdTypeVariable:
        typeString = tr("Variable");
        break;
    }
    appendSectionTitle(text, typeString);
    QStringList synonyms;
    QString synonym, synonymsText;
    foreach (synonym, command.names)
    {
        if (synonym != current->text(0))
            synonyms.append(CommandParameter::toMono(synonym));
    }
    if (!synonyms.empty())
        synonymsText = QString(" (%1 %2)").arg(
                    synonyms.length() == 1 ? tr("synonym:") : tr("synonyms:")).arg(synonyms.join(", "));
    text += QString("<p>%1%2</p>").arg(CommandParameter::toMono(current->text(0))).arg(synonymsText);
    if (!command.syntaxes.empty())
    {
        appendSectionTitle(text, tr("Usage"));
        InputSyntax syntax;
        foreach (syntax, command.syntaxes)
            text += formatInputSyntax(syntax);
    }
    QString desc(command.description("en")), example, relatedCommand;
    if (desc.length() > 0)
    {
        appendSectionTitle(text, tr("Description"));
        text += QString("<p>%2</p>").arg(desc);
    }
    if (command.references.length() > 0)
    {
        appendSectionTitle(text, tr("References"));
        Reference reference;
        foreach (reference, command.references)
            text += formatReference(reference);
    }
    QStringList links;
    foreach (relatedCommand, command.related)
        links.append(QString("<a href=\"%1\"><font face=\"freemono\">%1</font></a>").arg(relatedCommand));
    if (!links.isEmpty())
    {
        appendSectionTitle(text, tr("Related"));
        text += QString("<p>%1</p>").arg(links.join(", "));
    }
    if (!command.examples.isEmpty())
    {
        appendSectionTitle(text, tr("Examples"));
        foreach (example, command.examples)
            text += QString("<p style=\"font-family:freemono\">%1</p>").arg(example);
    }
    text += "</body></html>";
    ui->commandHelpBrowser->setText(text);
}

void CommandIndexDialog::on_commandHelpBrowser_anchorClicked(const QUrl &url)
{
    if (url.isValid())
        QDesktopServices::openUrl(url);
    else
    {
        QString commandName = url.toString();
        qInfo() << commandName;
    }
}
