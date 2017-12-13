#include "tablepropertiesdialog.h"
#include "ui_tablepropertiesdialog.h"

TablePropertiesDialog::TablePropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TablePropertiesDialog)
{
    ui->setupUi(this);
}

TablePropertiesDialog::~TablePropertiesDialog()
{
    delete ui;
}

int TablePropertiesDialog::tableRows()
{
    return ui->rowsSpinBox->value();
}

int TablePropertiesDialog::tableColumns()
{
    return ui->columnsSpinBox->value();
}

int TablePropertiesDialog::tableHeaderRows()
{
    return ui->headerRowsSpinBox->value();
}

int TablePropertiesDialog::tableFlags()
{
    Worksheet::TableFlags flags = Worksheet::None;
    if (ui->isNumericCheckBox->isChecked())
        flags |= Worksheet::Numeric;
    return flags;
}

void TablePropertiesDialog::on_rowsSpinBox_valueChanged(int arg1)
{
    ui->headerRowsSpinBox->setMaximum(arg1 - 1);
}
