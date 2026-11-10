#pragma once
#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;
class QTableView;
class QGridLayout;
class SlaModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void reload();
    void addIntervention();
    void editIntervention();
    void deleteIntervention();
    void archiveIntervention();
    void exportCsv();
    void exportHistoryPdf();
    void generateMissionOrder();
    void showSlaDelays();
    void predictivePlanning();
    void refreshStats();

private:
    QWidget *buildListTab();
    QWidget *buildStatsTab();
    QWidget *makeKpiCard(const QString &label, QLabel **valueOut);
    int selectedId() const;

    SlaModel   *m_model;
    QTableView *m_table;
    QLineEdit  *m_search;
    QComboBox  *m_sort, *m_statusFilter;
    QLabel *m_kpiTotal, *m_kpiLate, *m_kpiAvg, *m_kpiCost;
    QGridLayout *m_chartsLayout;
};
