#ifndef STATSVIEW_H
#define STATSVIEW_H

#include <QWidget>
#include <QMap>

class StatsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatsView(QWidget *parent = nullptr);

    void setDonneesGouvernorats(const QMap<QString, int> &donnees, int annee);
    void setDonneesSeverite(const QMap<QString, int> &donnees);
    void setDonneesTypes(const QMap<QString, int> &donnees);
    void refreshData(int annee = 2026);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QString, int> m_donneesGouv;
    QMap<QString, int> m_donneesSev;
    QMap<QString, int> m_donneesTypes;
    int m_annee;

    void drawBarChart(QPainter &p, const QRect &rect);
    void drawDonutSeverity(QPainter &p, const QRect &rect);
};

#endif // STATSVIEW_H
