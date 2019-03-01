#include "ulog_parameters_dialog.h"
#include "ui_ulog_parameters_dialog.h"

#include <QTableWidget>
#include <QSettings>
#include <QHeaderView>

ULogParametersDialog::ULogParametersDialog(const std::vector<std::pair<QString, QString> > &info,
                                           const std::vector<std::pair<QString, QString> > &params,
                                           QWidget *parent):
    QDialog(parent),
    ui(new Ui::ULogParametersDialog)
{
    ui->setupUi(this);
    QTableWidget* table_info = ui->tableWidgetInfo;
    QTableWidget* table_params = ui->tableWidgetParams;

    table_info->setRowCount(info.size());

    for(size_t row = 0; row < info.size(); row++ )
    {
        table_info->setItem( row, 0, new QTableWidgetItem( info[row].first ) );
        table_info->setItem( row, 1, new QTableWidgetItem( info[row].second ) );
    }
    table_info->sortItems(0);

    table_params->setRowCount(params.size());

    for(size_t row = 0; row < params.size(); row++ )
    {
        table_params->setItem( row, 0, new QTableWidgetItem( params[row].first ) );
        table_params->setItem( row, 1, new QTableWidgetItem( params[row].second ) );
    }
    table_params->sortItems(0);
}

void ULogParametersDialog::restoreSettings()
{
    QTableWidget* table_info = ui->tableWidgetInfo;
    QTableWidget* table_params = ui->tableWidgetParams;

    QSettings settings;
    restoreGeometry(settings.value("ULogParametersDialog/geometry").toByteArray());
    table_info->horizontalHeader()->restoreState(settings.value("ULogParametersDialog/info/state").toByteArray());
    table_params->horizontalHeader()->restoreState(settings.value("ULogParametersDialog/params/state").toByteArray());

    table_info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    table_info->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);

    table_params->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    table_params->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
}

ULogParametersDialog::~ULogParametersDialog()
{
    QTableWidget* table_info = ui->tableWidgetInfo;
    QTableWidget* table_params = ui->tableWidgetParams;

    QSettings settings;
    settings.setValue("ULogParametersDialog/geometry", this->saveGeometry() );
    settings.setValue("ULogParametersDialog/info/state",
                      table_info->horizontalHeader()->saveState() );
    settings.setValue("ULogParametersDialog/params/state",
                      table_params->horizontalHeader()->saveState() );

    delete ui;
}
