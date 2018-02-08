#ifndef COMMANDINDEXDIALOG_H
#define COMMANDINDEXDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "commandindex.h"

namespace Ui {
class CommandIndexDialog;
}

class CommandIndexDialog : public QDialog
{
    Q_OBJECT

    CommandIndex *commandIndex;
    QList<QTreeWidgetItem*> chapters;

    void populateIndex();
    QTreeWidgetItem *findChapter(const QString &name);
    QTreeWidgetItem *findSection(QTreeWidgetItem *chapter, const QString &name);
    void appendSectionTitle(QString &text, const QString &title);
    QString formatReference(const Reference &reference);
    QString formatInputSyntax(const InputSyntax &inputSyntax);

public:
    explicit CommandIndexDialog(CommandIndex *ci, QWidget *parent = 0);
    ~CommandIndexDialog();

private slots:

    void on_commandTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_commandHelpBrowser_anchorClicked(const QUrl &url);

private:
    Ui::CommandIndexDialog *ui;
};

#endif // COMMANDINDEXDIALOG_H
