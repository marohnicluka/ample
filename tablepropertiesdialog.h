#ifndef TABLEPROPERTIESDIALOG_H
#define TABLEPROPERTIESDIALOG_H

#include <QDialog>
#include "worksheet.h"

namespace Ui {
class TablePropertiesDialog;
}

class TablePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TablePropertiesDialog(QWidget *parent = 0);
    ~TablePropertiesDialog();
    int tableRows();
    int tableColumns();
    int tableHeaderRows();
    int tableFlags();

private slots:
    void on_rowsSpinBox_valueChanged(int arg1);

private:
    Ui::TablePropertiesDialog *ui;
};

#endif // TABLEPROPERTIESDIALOG_H
