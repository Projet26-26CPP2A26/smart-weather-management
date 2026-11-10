#include "statsview.h"
#include "alerte.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QFontMetrics>
#include <algorithm>

/* ===================================================================
   WIDGET PERSONNALISE DE VISUALISATION STATISTIQUE (DAYLIGHT CLEAR)
   ===================================================================
   NOTE PEDAGOGIQUE POUR ETUDIANTS EN INFORMATIQUE :
   - Ce widget dérive de QWidget et réimplémente paintEvent().
   - QPainter permet de dessiner des primitives géométriques (rectangles,
     lignes, camemberts/donuts, dégradés et textes).
   - Les calculs de coordonnées sont proportionnels à la largeur (width())
     et la hauteur (height()) pour garantir un rendu responsive fluide.
   - La palette de couleurs est réglée en mode "Daylight Clear" (fond clair,
     cartes blanches, textes ardoise foncés et accents azur vifs).
   =================================================================== */

StatsView::StatsView(QWidget *parent)
    : QWidget(parent), m_annee(2026)
{
    setMinimumSize(600, 360);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    refreshData(m_annee);
}

void StatsView::refreshData(int annee)
{
    m_annee = annee;
    m_donneesGouv = Alerte::statistiquesParGouvernorat(m_annee);
    m_donneesSev = Alerte::statistiquesParSeverite();
    m_donneesTypes = Alerte::statistiquesParType();
    update(); // Déclenche un réaffichage via paintEvent()
}

void StatsView::setDonneesGouvernorats(const QMap<QString, int> &donnees, int annee)
{
    m_donneesGouv = donnees;
    m_annee = annee;
    update();
}

void StatsView::setDonneesSeverite(const QMap<QString, int> &donnees)
{
    m_donneesSev = donnees;
    update();
}

void StatsView::setDonneesTypes(const QMap<QString, int> &donnees)
{
    m_donneesTypes = donnees;
    update();
}

void StatsView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // Lissage des contours vectoriels

    // 1. Fond global diurne clair (Daylight)
    p.fillRect(rect(), QColor(248, 250, 252)); // #f8fafc (Slate 50)

    // 2. Répartition de l'espace d'affichage
    if (width() > 700) {
        // Mode bi-panneau : 68% pour les barres, 32% pour le donut
        int barWidth = width() * 0.68;
        QRect rectBars(10, 10, barWidth - 15, height() - 20);
        QRect rectDonut(barWidth + 5, 10, width() - barWidth - 15, height() - 20);
        drawBarChart(p, rectBars);
        drawDonutSeverity(p, rectDonut);
    } else {
        // Mode écran étroit : affichage unique de l'histogramme
        drawBarChart(p, rect().adjusted(10, 10, -10, -10));
    }
}

void StatsView::drawBarChart(QPainter &p, const QRect &rect)
{
    // Cadre conteneur diurne : carte blanche avec bordure discrète
    p.setPen(QColor(226, 232, 240));  // #e2e8f0 (Bordure gris clair)
    p.setBrush(QColor(255, 255, 255)); // #ffffff (Fond blanc pur)
    p.drawRoundedRect(rect, 10, 10);

    // Titre de l'histogramme
    p.setPen(QColor(2, 132, 199)); // #0284c7 (Bleu azur diurne)
    QFont titleFont = p.font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.drawText(rect.adjusted(20, 15, -20, 0), Qt::AlignLeft | Qt::AlignTop, 
               QString("📊 Nombre d'Alertes Météorologiques par Gouvernorat (Année %1)").arg(m_annee));

    // Cas où aucune donnée n'est enregistrée
    if (m_donneesGouv.isEmpty()) {
        p.setPen(QColor(100, 116, 139)); // #64748b
        QFont f = p.font();
        f.setBold(false);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, "Aucune donnée disponible pour cette année.");
        return;
    }

    // Calcul de la valeur maximale pour dimensionner la hauteur des barres
    int maxVal = 1;
    for (int v : m_donneesGouv) {
        if (v > maxVal) maxVal = v;
    }
    maxVal = std::max(maxVal, 5); // Échelle minimale de 5 unités

    // Zone utile pour le graphique
    int chartLeft = rect.left() + 50;
    int chartRight = rect.right() - 20;
    int chartTop = rect.top() + 55;
    int chartBottom = rect.bottom() - 45;
    int chartHeight = chartBottom - chartTop;
    int chartWidth = chartRight - chartLeft;

    // Lignes de repère horizontales (grille diurne)
    p.setPen(QPen(QColor(203, 213, 225), 1, Qt::DashLine));
    QFont axisFont = p.font();
    axisFont.setPointSize(10);
    axisFont.setBold(true);
    p.setFont(axisFont);

    int steps = 4;
    for (int i = 0; i <= steps; ++i) {
        int y = chartBottom - (i * chartHeight / steps);
        int val = (i * maxVal) / steps;
        p.drawLine(chartLeft, y, chartRight, y);

        // Valeur de l'axe vertical (Y)
        p.setPen(QColor(15, 23, 42)); // #0f172a (Texte net et sombre)
        p.drawText(rect.left() + 5, y - 8, 40, 16, Qt::AlignRight | Qt::AlignVCenter, QString::number(val));
        p.setPen(QPen(QColor(203, 213, 225), 1, Qt::DashLine));
    }

    // Dessin de chaque barre
    int n = m_donneesGouv.size();
    if (n == 0) return;

    int slotWidth = chartWidth / n;
    int barWidth = std::max(16, std::min(38, slotWidth - 10));

    auto keys = m_donneesGouv.keys();
    for (int i = 0; i < n; ++i) {
        QString gouv = keys[i];
        int count = m_donneesGouv.value(gouv);

        int barH = (count * chartHeight) / maxVal;
        int x = chartLeft + (i * slotWidth) + (slotWidth - barWidth) / 2;
        int y = chartBottom - barH;

        QRect barRect(x, y, barWidth, barH);

        // Dégradé azur diurne moderne
        QLinearGradient grad(x, y, x, chartBottom);
        grad.setColorAt(0.0, QColor(14, 165, 233)); // #0ea5e9
        grad.setColorAt(1.0, QColor(2, 132, 199));  // #0284c7

        p.setPen(Qt::NoPen);
        p.setBrush(grad);
        p.drawRoundedRect(barRect, 4, 4);

        // Valeur numérique affichée au-dessus de la barre
        p.setPen(QColor(15, 23, 42)); // #0f172a (Ardoise sombre net)
        QFont valFont = p.font();
        valFont.setPointSize(10);
        valFont.setBold(true);
        p.setFont(valFont);
        p.drawText(x - 5, y - 20, barWidth + 10, 18, Qt::AlignCenter, QString::number(count));

        // Nom du gouvernorat (axe horizontal X)
        p.setPen(QColor(15, 23, 42)); // #0f172a
        QFont labelFont = p.font();
        labelFont.setPointSize(9);
        labelFont.setBold(true);
        p.setFont(labelFont);

        QString shortName = gouv;
        if (shortName.length() > 8) {
            shortName = shortName.left(7) + ".";
        }
        p.drawText(x - 8, chartBottom + 6, barWidth + 16, 25, Qt::AlignCenter | Qt::TextWordWrap, shortName);
    }
}

void StatsView::drawDonutSeverity(QPainter &p, const QRect &rect)
{
    // Cadre conteneur diurne
    p.setPen(QColor(226, 232, 240));
    p.setBrush(QColor(255, 255, 255));
    p.drawRoundedRect(rect, 10, 10);

    // Titre de la répartition
    p.setPen(QColor(2, 132, 199));
    QFont titleFont = p.font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.drawText(rect.adjusted(15, 15, -15, 0), Qt::AlignLeft | Qt::AlignTop, "🍩 Répartition par Sévérité");

    int total = 0;
    for (int v : m_donneesSev) total += v;

    if (total == 0) {
        p.setPen(QColor(15, 23, 42));
        QFont f = p.font();
        f.setPointSize(11);
        f.setBold(true);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, "Pas de données");
        return;
    }

    int rouge = m_donneesSev.value("Rouge", 0);
    int orange = m_donneesSev.value("Orange", 0);
    int jaune = m_donneesSev.value("Jaune", 0);

    // Dimensions géométriques du donut
    int centerX = rect.center().x();
    int centerY = rect.top() + 110;
    int outerRadius = 55;
    int innerRadius = 32;
    QRect donutRect(centerX - outerRadius, centerY - outerRadius, outerRadius * 2, outerRadius * 2);

    int startAngle = 90 * 16; // 16 unités par degré sous Qt
    auto drawSlice = [&](int val, const QColor &color) {
        if (val <= 0) return;
        int spanAngle = - (val * 360 * 16) / total;
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawPie(donutRect, startAngle, spanAngle);
        startAngle += spanAngle;
    };

    drawSlice(rouge, QColor(239, 68, 68));   // Rouge vif
    drawSlice(orange, QColor(249, 115, 22)); // Orange
    drawSlice(jaune, QColor(234, 179, 8));   // Jaune ambré

    // Trou central blanc (donne l'effet Donut)
    p.setBrush(QColor(255, 255, 255));
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPoint(centerX, centerY), innerRadius, innerRadius);

    // Valeur numérique centrale (Total)
    p.setPen(QColor(15, 23, 42)); // #0f172a
    QFont totalFont = p.font();
    totalFont.setPointSize(15);
    totalFont.setBold(true);
    p.setFont(totalFont);
    p.drawText(QRect(centerX - 35, centerY - 15, 70, 30), Qt::AlignCenter, QString::number(total));

    // Légende sous le donut
    int legendY = rect.bottom() - 110;
    auto drawLegendItem = [&](const QString &label, int count, const QColor &col, int y) {
        // Pastille de couleur
        p.setPen(Qt::NoPen);
        p.setBrush(col);
        p.drawRoundedRect(rect.left() + 20, y + 2, 14, 14, 4, 4);

        // Intitulé
        p.setPen(QColor(15, 23, 42)); // #0f172a (bien noir et lisible)
        QFont lf = p.font();
        lf.setPointSize(11);
        lf.setBold(true);
        p.setFont(lf);
        p.drawText(rect.left() + 42, y, rect.width() - 85, 18, Qt::AlignLeft | Qt::AlignVCenter, label);

        // Nombre
        p.setPen(QColor(15, 23, 42)); // #0f172a
        p.setFont(lf);
        p.drawText(rect.right() - 55, y, 45, 18, Qt::AlignRight | Qt::AlignVCenter, QString::number(count));
    };

    drawLegendItem("Rouge (Critique)", rouge, QColor(239, 68, 68), legendY);
    drawLegendItem("Orange (Élevé)", orange, QColor(249, 115, 22), legendY + 28);
    drawLegendItem("Jaune (Modéré)", jaune, QColor(234, 179, 8), legendY + 56);
}
