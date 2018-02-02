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

void CommandIndexDialog::on_commandTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current->childCount() > 0)
        return;
    int index = current->data(0, Qt::UserRole).toInt();
    const Command &command = commandIndex->commandAt(index);
    QString text("<html><style>radicand{text-decoration:overline;}</style><body>");
    QString desc(command.descriptions["en"]), refs, example, relatedCommand;
    if (desc.length() > 0)
    {
        appendSectionTitle(text, tr("Description"));
        text += QString("<p>%2</p>").arg(desc);
    }
    if ((refs = command.references["all"]).length() > 0)
    {
        appendSectionTitle(text, tr("References"));
        text += command.references["all"];
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
