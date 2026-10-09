#include "smartweather.h"

#include <QCalendarWidget>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDate>
#include <QDateEdit>
#include <QFile>
#include <QFileDialog>
#include <QFontMetrics>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QLocale>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPainterPath>
#include <QPair>
#include <QPdfWriter>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QStackedWidget>
#include <QTableView>
#include <QTextDocument>
#include <QTextStream>
#include <QUuid>
#include <QVBoxLayout>
#include <QVector>
#include <QtMath>

enum Col { C_ID, C_NOM, C_PRENOM, C_EMAIL, C_POSTE, C_STRUCT, C_GOUV, C_DATE, C_STATUT, C_HORAIRE };

static const QStringList ROLES_EMPLOYES = {"Responsable RH", "Chef de division", "Responsable régional"};

class SwRing : public QWidget
{
public:
    SwRing(const QColor &c1, const QColor &c2, QWidget *parent = nullptr)
        : QWidget(parent), m_c1(c1), m_c2(c2)
    {
        setMinimumSize(120, 120);
    }

    void setValue(double v, const QString &sub = QString())
    {
        m_v = qBound(0.0, v, 100.0);
        m_sub = sub;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const int side = qMin(width(), height()) - 10;
        const double w = side * 0.12;
        const QRectF r((width() - side) / 2.0 + w / 2, (height() - side) / 2.0 + w / 2,
                       side - w, side - w);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#E3EAF2"), w));
        p.drawEllipse(r);
        if (m_v > 0) {
            QLinearGradient g(r.topLeft(), r.bottomRight());
            g.setColorAt(0, m_c1);
            g.setColorAt(1, m_c2);
            p.setPen(QPen(QBrush(g), w, Qt::SolidLine, Qt::RoundCap));
            p.drawArc(r, 90 * 16, int(-m_v * 3.6 * 16));
        }
        QFont f = font();
        f.setBold(true);
        f.setPixelSize(qMax(14, side / 5));
        p.setFont(f);
        p.setPen(QColor("#37474F"));
        const QString txt = QString::number(qRound(m_v)) + "%";
        if (m_sub.isEmpty()) {
            p.drawText(r, Qt::AlignCenter, txt);
        } else {
            p.drawText(QRectF(r.left(), r.top(), r.width(), r.height() * 0.58),
                       Qt::AlignHCenter | Qt::AlignBottom, txt);
            QFont s = font();
            s.setPixelSize(qMax(10, side / 11));
            p.setFont(s);
            p.setPen(QColor("#78909C"));
            p.drawText(QRectF(r.left(), r.top() + r.height() * 0.6, r.width(), r.height() * 0.3),
                       Qt::AlignHCenter | Qt::AlignTop, m_sub);
        }
    }

private:
    QColor m_c1, m_c2;
    double m_v = 0;
    QString m_sub;
};

class SwLineChart : public QWidget
{
public:
    explicit SwLineChart(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(210);
    }

    void setData(const QVector<QPair<QString, double>> &pts)
    {
        m_pts = pts;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_pts.isEmpty()) {
            p.setPen(QColor("#90A4AE"));
            p.drawText(rect(), Qt::AlignCenter, "Aucune donnée pour le moment");
            return;
        }
        const QRectF area(42, 18, width() - 42 - 24, height() - 18 - 34);
        double mx = 0;
        for (const auto &pt : m_pts) mx = qMax(mx, pt.second);
        const int top = qMax(4, int(qCeil(mx / 4.0)) * 4);

        QFont f = font();
        f.setPixelSize(11);
        p.setFont(f);
        for (int i = 0; i <= 4; ++i) {
            const double y = area.bottom() - area.height() * i / 4.0;
            p.setPen(QPen(QColor("#E8EEF4"), 1));
            p.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
            p.setPen(QColor("#90A4AE"));
            p.drawText(QRectF(0, y - 9, area.left() - 8, 18),
                       Qt::AlignRight | Qt::AlignVCenter, QString::number(top * i / 4));
        }

        const int n = m_pts.size();
        QVector<QPointF> xy;
        for (int i = 0; i < n; ++i) {
            const double x = n == 1 ? area.center().x() : area.left() + area.width() * i / (n - 1);
            const double y = area.bottom() - area.height() * m_pts[i].second / top;
            xy.append(QPointF(x, y));
        }

        if (n > 1) {
            QPainterPath path(xy[0]);
            for (int i = 1; i < n; ++i) {
                const double mid = xy[i - 1].x() + (xy[i].x() - xy[i - 1].x()) / 2;
                path.cubicTo(QPointF(mid, xy[i - 1].y()), QPointF(mid, xy[i].y()), xy[i]);
            }
            QPainterPath fill = path;
            fill.lineTo(xy.last().x(), area.bottom());
            fill.lineTo(xy.first().x(), area.bottom());
            fill.closeSubpath();
            QLinearGradient g(0, area.top(), 0, area.bottom());
            g.setColorAt(0, QColor(30, 136, 229, 95));
            g.setColorAt(1, QColor(30, 136, 229, 0));
            p.fillPath(fill, g);
            p.setPen(QPen(QColor("#1E88E5"), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);
        }

        for (int i = 0; i < n; ++i) {
            p.setPen(QPen(QColor("#1E88E5"), 2));
            p.setBrush(Qt::white);
            p.drawEllipse(xy[i], 5, 5);
            QFont b = f;
            b.setBold(true);
            p.setFont(b);
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(xy[i].x() - 24, xy[i].y() - 26, 48, 18), Qt::AlignCenter,
                       QString::number(int(m_pts[i].second)));
            p.setFont(f);
            p.setPen(QColor("#78909C"));
            p.drawText(QRectF(xy[i].x() - 30, area.bottom() + 10, 60, 18), Qt::AlignCenter,
                       m_pts[i].first);
        }
    }

private:
    QVector<QPair<QString, double>> m_pts;
};

class SwBarChart : public QWidget
{
public:
    explicit SwBarChart(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(210);
    }

    void setData(const QVector<QPair<QString, double>> &items)
    {
        m_items = items;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_items.isEmpty()) {
            p.setPen(QColor("#90A4AE"));
            p.drawText(rect(), Qt::AlignCenter, "Aucune donnée pour le moment");
            return;
        }
        double mx = 1;
        for (const auto &it : m_items) mx = qMax(mx, it.second);
        const int n = m_items.size();
        const double rowH = qMin(42.0, height() / double(n));
        const double y0 = (height() - rowH * n) / 2.0;
        const double labelW = qMin(170.0, width() * 0.42);
        const double barX = labelW + 10;
        const double barW = width() - barX - 40;
        QFont f = font();
        f.setPixelSize(12);
        p.setFont(f);
        QFontMetrics fm(f);
        static const char *debut[] = {"#1E88E5", "#2196F3", "#42A5F5", "#64B5F6", "#90CAF9"};
        static const char *fin[] = {"#1565C0", "#1E88E5", "#1E88E5", "#42A5F5", "#64B5F6"};
        for (int i = 0; i < n; ++i) {
            const double y = y0 + rowH * i;
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(0, y, labelW, rowH), Qt::AlignRight | Qt::AlignVCenter,
                       fm.elidedText(m_items[i].first, Qt::ElideRight, int(labelW)));
            const QRectF track(barX, y + rowH / 2 - 7, barW, 14);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#EEF3F8"));
            p.drawRoundedRect(track, 7, 7);
            const double w = qMax(14.0, barW * m_items[i].second / mx);
            QLinearGradient g(track.left(), 0, track.left() + w, 0);
            g.setColorAt(0, QColor(debut[i % 5]));
            g.setColorAt(1, QColor(fin[i % 5]));
            p.setBrush(g);
            p.drawRoundedRect(QRectF(track.left(), track.top(), w, track.height()), 7, 7);
            p.setPen(QColor("#37474F"));
            p.drawText(QRectF(barX + barW + 6, y, 34, rowH), Qt::AlignLeft | Qt::AlignVCenter,
                       QString::number(int(m_items[i].second)));
        }
    }

private:
    QVector<QPair<QString, double>> m_items;
};

static QFrame *makeCard(QWidget *content, const QString &titre = QString())
{
    auto *card = new QFrame;
    card->setObjectName("card");
    auto *l = new QVBoxLayout(card);
    l->setContentsMargins(20, 16, 20, 18);
    l->setSpacing(10);
    if (!titre.isEmpty()) {
        auto *t = new QLabel(titre);
        t->setObjectName("cardTitle");
        l->addWidget(t);
    }
    l->addWidget(content, 1);
    auto *sh = new QGraphicsDropShadowEffect(card);
    sh->setBlurRadius(30);
    sh->setOffset(0, 6);
    sh->setColor(QColor(38, 70, 110, 45));
    card->setGraphicsEffect(sh);
    return card;
}

smartweather::smartweather(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Smart Weather Management");
    resize(1320, 800);
    setMinimumSize(1150, 700);
    if (!initDb()) return;

    m_root = new QStackedWidget;
    m_root->addWidget(buildLoginPage());
    m_root->addWidget(buildShell());

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_root);

    appliquerStyle();
    m_root->setCurrentIndex(0);
}

bool smartweather::initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("smartweather.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Base de données", db.lastError().text());
        return false;
    }
    QSqlQuery q;
    bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS employe ("
        "id_employe INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nom TEXT NOT NULL, prenom TEXT NOT NULL, email TEXT NOT NULL,"
        "poste TEXT, structure TEXT, gouvernorat TEXT,"
        "date_recrutement TEXT, statut TEXT, type_horaire TEXT)");
    ok = ok && q.exec(
             "CREATE TABLE IF NOT EXISTS utilisateur ("
             "id_utilisateur INTEGER PRIMARY KEY AUTOINCREMENT,"
             "login TEXT UNIQUE NOT NULL, mot_de_passe TEXT NOT NULL,"
             "sel TEXT NOT NULL, role TEXT NOT NULL)");
    if (ok) initUtilisateurs();
    return ok;
}

QString smartweather::hasher(const QString &motDePasse, const QString &sel)
{
    const QByteArray h = QCryptographicHash::hash((sel + motDePasse).toUtf8(),
                                                  QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

void smartweather::initUtilisateurs()
{
    QSqlQuery c("SELECT COUNT(*) FROM utilisateur");
    if (c.next() && c.value(0).toInt() > 0) return;

    const struct { const char *login, *mdp, *role; } comptes[] = {
                    {"admin",     "admin123", "Responsable RH"},
                    {"prevision", "prev123",  "Prévisionniste"},
                    };
    for (const auto &u : comptes) {
        const QString sel = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QSqlQuery q;
        q.prepare("INSERT INTO utilisateur (login, mot_de_passe, sel, role) VALUES (?,?,?,?)");
        q.addBindValue(QString::fromUtf8(u.login));
        q.addBindValue(hasher(QString::fromUtf8(u.mdp), sel));
        q.addBindValue(sel);
        q.addBindValue(QString::fromUtf8(u.role));
        q.exec();
    }
}

QWidget *smartweather::buildLoginPage()
{
    auto *page = new QWidget;

    auto *hero = new QFrame;
    hero->setObjectName("hero");
    auto *hl = new QVBoxLayout(hero);
    hl->setContentsMargins(70, 60, 70, 60);
    auto *logo = new QLabel("☁");
    logo->setStyleSheet("font-size:84pt;");
    auto *htitre = new QLabel("Smart Weather\nManagement");
    htitre->setStyleSheet("font-size:34pt; font-weight:bold;");
    auto *hsous = new QLabel("Supervision du réseau d'observation de l'Institut National de la Météorologie : "
                             "personnel, stations, relevés et alertes au même endroit.");
    hsous->setWordWrap(true);
    hsous->setStyleSheet("font-size:13pt;");
    hl->addStretch();
    hl->addWidget(logo);
    hl->addWidget(htitre);
    hl->addSpacing(12);
    hl->addWidget(hsous);
    hl->addStretch();

    m_loginUser = new QLineEdit;
    m_loginUser->setObjectName("loginUser");
    m_loginUser->setPlaceholderText("Identifiant");
    m_loginPass = new QLineEdit;
    m_loginPass->setObjectName("loginPass");
    m_loginPass->setPlaceholderText("Mot de passe");
    m_loginPass->setEchoMode(QLineEdit::Password);
    m_loginErreur = new QLabel;
    m_loginErreur->setObjectName("erreur");
    auto *btn = new QPushButton("Se connecter");
    btn->setObjectName("btnLogin");
    btn->setCursor(Qt::PointingHandCursor);

    auto *ctitre = new QLabel("Connexion");
    ctitre->setObjectName("pageTitle");
    auto *csous = new QLabel("Entrez vos identifiants pour accéder à l'application.");
    csous->setObjectName("kpiSub");
    csous->setWordWrap(true);

    auto *form = new QWidget;
    form->setFixedWidth(380);
    auto *fl = new QVBoxLayout(form);
    fl->setContentsMargins(0, 0, 0, 0);
    fl->setSpacing(14);
    fl->addWidget(ctitre);
    fl->addWidget(csous);
    fl->addSpacing(10);
    fl->addWidget(m_loginUser);
    fl->addWidget(m_loginPass);
    fl->addWidget(m_loginErreur);
    fl->addWidget(btn);

    auto *right = new QWidget;
    auto *rl = new QHBoxLayout(right);
    rl->addStretch();
    rl->addWidget(form);
    rl->addStretch();

    auto *h = new QHBoxLayout(page);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);
    h->addWidget(hero, 5);
    h->addWidget(right, 4);

    connect(btn, &QPushButton::clicked, this, &smartweather::seConnecter);
    connect(m_loginPass, &QLineEdit::returnPressed, this, &smartweather::seConnecter);
    connect(m_loginUser, &QLineEdit::returnPressed, m_loginPass, QOverload<>::of(&QWidget::setFocus));
    return page;
}

void smartweather::seConnecter()
{
    const QString user = m_loginUser->text().trimmed();
    const QString mdp = m_loginPass->text();

    QSqlQuery q;
    q.prepare("SELECT mot_de_passe, sel, role FROM utilisateur WHERE login=?");
    q.addBindValue(user);
    if (!q.exec() || !q.next() || hasher(mdp, q.value(1).toString()) != q.value(0).toString()) {
        m_loginErreur->setText("Identifiant ou mot de passe incorrect.");
        m_loginPass->clear();
        return;
    }

    m_user = user;
    m_role = q.value(2).toString();
    m_loginErreur->clear();
    m_loginUser->clear();
    m_loginPass->clear();

    m_userNom->setText(m_user);
    m_userRole->setText(m_role);
    m_avatar->setText(m_user.left(1).toUpper());
    const bool autorise = ROLES_EMPLOYES.contains(m_role);
    m_navEmployes->setEnabled(autorise);
    m_navEmployes->setToolTip(autorise ? QString() : "Accès réservé aux responsables RH, chefs de division et responsables régionaux.");

    m_root->setCurrentIndex(1);
    allerPage(0);
}

void smartweather::deconnecter()
{
    m_user.clear();
    m_role.clear();
    m_root->setCurrentIndex(0);
    m_loginUser->setFocus();
}

QWidget *smartweather::buildShell()
{
    auto *shell = new QWidget;
    auto *h = new QHBoxLayout(shell);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);

    h->addWidget(buildSidebar());

    auto *contenu = new QWidget;
    auto *cl = new QVBoxLayout(contenu);
    cl->setContentsMargins(28, 20, 28, 24);
    cl->setSpacing(18);
    cl->addWidget(buildHeader());

    m_pages = new QStackedWidget;
    m_pages->addWidget(buildDashboardPage());
    m_pages->addWidget(buildEmployesPage());
    m_pages->addWidget(buildPlaceholder("Gestion des stations"));
    m_pages->addWidget(buildPlaceholder("Relevés météo & attestations"));
    m_pages->addWidget(buildPlaceholder("Alertes & prédictions"));
    cl->addWidget(m_pages, 1);

    h->addWidget(contenu, 1);
    return shell;
}

QWidget *smartweather::buildSidebar()
{
    auto *bar = new QFrame;
    bar->setObjectName("sidebar");
    bar->setFixedWidth(235);
    auto *l = new QVBoxLayout(bar);
    l->setContentsMargins(0, 24, 0, 20);
    l->setSpacing(4);

    auto *brand = new QLabel("☁  Smart Weather");
    brand->setObjectName("brand");
    auto *inst = new QLabel("INM  •  Tunisie");
    inst->setObjectName("brandSub");
    l->addWidget(brand);
    l->addWidget(inst);
    l->addSpacing(26);

    const QStringList noms = {"Tableau de bord", "Employés", "Stations",
                              "Relevés && Attestations", "Alertes && Prédictions"};
    for (int i = 0; i < noms.size(); ++i) {
        auto *b = new QPushButton(noms[i]);
        b->setObjectName("nav");
        b->setCheckable(true);
        b->setAutoExclusive(true);
        b->setCursor(Qt::PointingHandCursor);
        l->addWidget(b);
        m_nav.append(b);
        connect(b, &QPushButton::clicked, this, [this, i] { allerPage(i); });
    }
    m_navEmployes = m_nav[1];
    m_nav[0]->setChecked(true);

    l->addStretch();
    auto *logout = new QPushButton("Déconnexion");
    logout->setObjectName("logout");
    logout->setCursor(Qt::PointingHandCursor);
    auto *wrap = new QHBoxLayout;
    wrap->setContentsMargins(18, 0, 18, 0);
    wrap->addWidget(logout);
    l->addLayout(wrap);
    connect(logout, &QPushButton::clicked, this, &smartweather::deconnecter);
    return bar;
}

QWidget *smartweather::buildHeader()
{
    auto *w = new QWidget;
    auto *l = new QHBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(14);

    m_titrePage = new QLabel("Tableau de bord");
    m_titrePage->setObjectName("pageTitle");

    m_rechercheGlobale = new QLineEdit;
    m_rechercheGlobale->setObjectName("globalSearch");
    m_rechercheGlobale->setPlaceholderText("Rechercher un employé par nom...");

    m_avatar = new QLabel("?");
    m_avatar->setObjectName("avatar");
    m_avatar->setFixedSize(40, 40);
    m_avatar->setAlignment(Qt::AlignCenter);
    m_userNom = new QLabel;
    m_userNom->setObjectName("userNom");
    m_userRole = new QLabel;
    m_userRole->setObjectName("kpiSub");
    auto *infos = new QVBoxLayout;
    infos->setSpacing(0);
    infos->addWidget(m_userNom);
    infos->addWidget(m_userRole);

    l->addWidget(m_titrePage);
    l->addStretch();
    l->addWidget(m_rechercheGlobale);
    l->addSpacing(8);
    l->addWidget(m_avatar);
    l->addLayout(infos);

    connect(m_rechercheGlobale, &QLineEdit::returnPressed, this, &smartweather::rechercheGlobale);
    return w;
}

QWidget *smartweather::buildPlaceholder(const QString &nom)
{
    auto *lab = new QLabel(QString("Module « %1 »\nÀ brancher ici.").arg(nom));
    lab->setAlignment(Qt::AlignCenter);
    lab->setObjectName("kpiSub");
    lab->setStyleSheet("font-size:14pt;");
    auto *page = new QWidget;
    auto *l = new QVBoxLayout(page);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(makeCard(lab));
    return page;
}

void smartweather::allerPage(int index)
{
    static const QStringList titres = {"Tableau de bord", "Gestion des employés", "Gestion des stations",
                                       "Relevés météo & attestations", "Alertes & prédictions"};
    if (index < 0 || index >= m_pages->count()) return;
    m_pages->setCurrentIndex(index);
    m_titrePage->setText(titres.value(index));
    m_nav.value(index)->setChecked(true);
    if (index == 0) rafraichirDashboard();
    if (index == 1) charger();
}

void smartweather::rechercheGlobale()
{
    if (!m_navEmployes->isEnabled()) {
        QMessageBox::information(this, "Accès", "Votre rôle n'a pas accès au module Employés.");
        return;
    }
    const QString t = m_rechercheGlobale->text().trimmed();
    m_critere->setCurrentIndex(0);
    m_recherche->setText(t);
    m_rechercheGlobale->clear();
    allerPage(1);
}

QWidget *smartweather::buildDashboardPage()
{
    auto *content = new QWidget;
    auto *g = new QGridLayout(content);
    g->setContentsMargins(0, 0, 8, 8);
    g->setSpacing(18);

    auto *kw = new QWidget;
    auto *kl = new QVBoxLayout(kw);
    kl->setContentsMargins(0, 0, 0, 0);
    auto *kt = new QLabel("Effectif total");
    kt->setObjectName("cardTitle");
    m_kpiTotal = new QLabel("0");
    m_kpiTotal->setObjectName("kpiValue");
    m_kpiTotalSous = new QLabel;
    m_kpiTotalSous->setObjectName("kpiSub");
    m_kpiTotalSous->setWordWrap(true);
    kl->addWidget(kt);
    kl->addStretch();
    kl->addWidget(m_kpiTotal);
    kl->addWidget(m_kpiTotalSous);
    kl->addStretch();

    m_ringActifs = new SwRing(QColor("#64B5F6"), QColor("#1565C0"));
    m_ringGarde = new SwRing(QColor("#4DD0E1"), QColor("#1E88E5"));
    m_ringConge = new SwRing(QColor("#90CAF9"), QColor("#1E88E5"));
    m_ringSiege = new SwRing(QColor("#64B5F6"), QColor("#1565C0"));
    m_ringRegionale = new SwRing(QColor("#4DD0E1"), QColor("#1E88E5"));

    auto ringCard = [](const QString &titre, SwRing *ring) {
        auto *w = new QWidget;
        auto *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->addWidget(ring, 1);
        return makeCard(w, titre);
    };

    g->addWidget(makeCard(kw), 0, 0);
    g->addWidget(ringCard("Employés actifs", m_ringActifs), 0, 1);
    g->addWidget(ringCard("Gardes 24h/24", m_ringGarde), 0, 2);
    g->addWidget(ringCard("En congé", m_ringConge), 0, 3);

    m_courbe = new SwLineChart;
    m_barres = new SwBarChart;
    g->addWidget(makeCard(m_courbe, "Recrutements par année"), 1, 0, 1, 2);
    g->addWidget(makeCard(m_barres, "Employés par poste"), 1, 2, 1, 2);

    m_activites = new QLabel;
    m_activites->setTextFormat(Qt::RichText);
    m_activites->setWordWrap(true);
    m_activites->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_activites->setMinimumHeight(220);
    g->addWidget(makeCard(m_activites, "Activités récentes"), 2, 0);

    auto *cal = new QCalendarWidget;
    cal->setLocale(QLocale(QLocale::French));
    cal->setGridVisible(false);
    cal->setFirstDayOfWeek(Qt::Monday);
    cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    cal->setMinimumHeight(220);
    g->addWidget(makeCard(cal, "Calendrier"), 2, 1);

    auto *sw = new QWidget;
    auto *sl = new QHBoxLayout(sw);
    sl->setContentsMargins(0, 0, 0, 0);
    auto colonne = [](SwRing *ring, const QString &nom) {
        auto *c = new QWidget;
        auto *l = new QVBoxLayout(c);
        l->setContentsMargins(0, 0, 0, 0);
        l->addWidget(ring, 1);
        auto *t = new QLabel(nom);
        t->setObjectName("kpiSub");
        t->setAlignment(Qt::AlignCenter);
        l->addWidget(t);
        return c;
    };
    sl->addWidget(colonne(m_ringSiege, "Siège"));
    sl->addWidget(colonne(m_ringRegionale, "Régionale"));
    g->addWidget(makeCard(sw, "Répartition par structure"), 2, 2, 1, 2);

    for (int c = 0; c < 4; ++c) g->setColumnStretch(c, 1);
    g->setRowStretch(0, 3);
    g->setRowStretch(1, 4);
    g->setRowStretch(2, 4);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(content);
    return scroll;
}

void smartweather::rafraichirDashboard()
{
    auto scalaire = [](const QString &sql) {
        QSqlQuery q(sql);
        return q.next() ? q.value(0).toInt() : 0;
    };
    const int total = scalaire("SELECT COUNT(*) FROM employe");
    const int actifs = scalaire("SELECT COUNT(*) FROM employe WHERE statut='Actif'");
    const int conge = scalaire("SELECT COUNT(*) FROM employe WHERE statut='En congé'");
    const int stagiaires = scalaire("SELECT COUNT(*) FROM employe WHERE statut='Stagiaire'");
    const int garde = scalaire("SELECT COUNT(*) FROM employe WHERE type_horaire='Garde 24h'");
    const int siege = scalaire("SELECT COUNT(*) FROM employe WHERE structure='Siège'");
    const int regionale = scalaire("SELECT COUNT(*) FROM employe WHERE structure='Régionale'");
    auto pct = [total](int n) { return total ? 100.0 * n / total : 0.0; };
    auto sur = [total](int n) { return QString("%1 / %2").arg(n).arg(total); };

    m_kpiTotal->setText(QString::number(total));
    m_kpiTotalSous->setText(QString("employés enregistrés\n%1 stagiaires").arg(stagiaires));
    m_ringActifs->setValue(pct(actifs), sur(actifs));
    m_ringGarde->setValue(pct(garde), sur(garde));
    m_ringConge->setValue(pct(conge), sur(conge));
    m_ringSiege->setValue(pct(siege), sur(siege));
    m_ringRegionale->setValue(pct(regionale), sur(regionale));

    QVector<QPair<QString, double>> annees;
    QSqlQuery qa("SELECT substr(date_recrutement,1,4) AS an, COUNT(*) FROM employe "
                 "WHERE date_recrutement IS NOT NULL AND date_recrutement<>'' GROUP BY an ORDER BY an");
    while (qa.next()) annees.append({qa.value(0).toString(), qa.value(1).toDouble()});
    while (annees.size() > 8) annees.removeFirst();
    m_courbe->setData(annees);

    QVector<QPair<QString, double>> postes;
    QSqlQuery qp("SELECT poste, COUNT(*) AS c FROM employe GROUP BY poste ORDER BY c DESC LIMIT 5");
    while (qp.next()) postes.append({qp.value(0).toString(), qp.value(1).toDouble()});
    m_barres->setData(postes);

    QString html;
    QSqlQuery qr("SELECT nom, prenom, poste, date_recrutement FROM employe ORDER BY id_employe DESC LIMIT 6");
    while (qr.next()) {
        const QDate d = QDate::fromString(qr.value(3).toString(), "yyyy-MM-dd");
        html += QString("<p style='margin:0 0 9px 0;'><b>%1 %2</b><br>"
                        "<span style='color:#78909C;'>%3 • %4</span></p>")
                    .arg(qr.value(0).toString().toHtmlEscaped(), qr.value(1).toString().toHtmlEscaped(),
                         qr.value(2).toString().toHtmlEscaped(), d.toString("dd/MM/yyyy"));
    }
    if (html.isEmpty()) html = "<span style='color:#90A4AE;'>Aucun employé enregistré.</span>";
    m_activites->setText(html);
}

QWidget *smartweather::buildEmployesPage()
{
    auto *page = new QWidget;

    m_nom = new QLineEdit;
    m_prenom = new QLineEdit;
    m_email = new QLineEdit;
    m_gouvernorat = new QLineEdit;
    m_poste = new QComboBox;
    m_poste->addItems({"Ingénieur prévisionniste", "Agent d'observation",
                       "Technicien de maintenance", "Agent de service", "Administratif"});
    m_structure = new QComboBox;
    m_structure->addItems({"Siège", "Régionale"});
    m_statut = new QComboBox;
    m_statut->addItems({"Actif", "En congé", "Stagiaire"});
    m_horaire = new QComboBox;
    m_horaire->addItems({"Jour", "Garde 24h"});
    m_date = new QDateEdit(QDate::currentDate());
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat("dd/MM/yyyy");

    auto *form = new QFormLayout;
    form->setVerticalSpacing(10);
    form->addRow("Nom", m_nom);
    form->addRow("Prénom", m_prenom);
    form->addRow("Email", m_email);
    form->addRow("Poste", m_poste);
    form->addRow("Structure", m_structure);
    form->addRow("Gouvernorat", m_gouvernorat);
    form->addRow("Recrutement", m_date);
    form->addRow("Statut", m_statut);
    form->addRow("Horaire", m_horaire);

    auto *btnAjouter = new QPushButton("Ajouter");
    auto *btnModifier = new QPushButton("Modifier");
    auto *btnSupprimer = new QPushButton("Supprimer");
    auto *btnVider = new QPushButton("Nouveau");
    btnSupprimer->setObjectName("danger");
    for (auto *b : {btnAjouter, btnModifier, btnSupprimer, btnVider}) b->setCursor(Qt::PointingHandCursor);
    auto *btns = new QGridLayout;
    btns->setSpacing(10);
    btns->addWidget(btnAjouter, 0, 0);
    btns->addWidget(btnModifier, 0, 1);
    btns->addWidget(btnSupprimer, 1, 0);
    btns->addWidget(btnVider, 1, 1);

    auto *formW = new QWidget;
    auto *fl = new QVBoxLayout(formW);
    fl->setContentsMargins(0, 0, 0, 0);
    fl->addLayout(form);
    fl->addSpacing(8);
    fl->addLayout(btns);
    fl->addStretch();
    auto *formCard = makeCard(formW, "Fiche employé");
    formCard->setFixedWidth(370);

    m_critere = new QComboBox;
    m_critere->addItems({"Nom", "Poste", "Structure", "Gouvernorat"});
    m_recherche = new QLineEdit;
    m_recherche->setPlaceholderText("Rechercher...");
    m_tri = new QComboBox;
    m_tri->addItems({"Nom", "Poste", "Date de recrutement", "Structure"});

    auto *btnCsv = new QPushButton("Excel (CSV)");
    auto *btnPdf = new QPushButton("PDF");
    auto *btnStats = new QPushButton("Statistiques");
    for (auto *b : {btnCsv, btnPdf, btnStats}) b->setCursor(Qt::PointingHandCursor);

    auto *titre = new QLabel("Liste des employés");
    titre->setObjectName("cardTitle");
    auto *ligne1 = new QHBoxLayout;
    ligne1->setSpacing(8);
    ligne1->addWidget(titre);
    ligne1->addStretch();
    ligne1->addWidget(btnCsv);
    ligne1->addWidget(btnPdf);
    ligne1->addWidget(btnStats);

    auto *ligne2 = new QHBoxLayout;
    ligne2->setSpacing(8);
    ligne2->addWidget(new QLabel("Rechercher par"));
    ligne2->addWidget(m_critere);
    ligne2->addWidget(m_recherche, 1);
    ligne2->addSpacing(10);
    ligne2->addWidget(new QLabel("Trier par"));
    ligne2->addWidget(m_tri);

    m_model = new QSqlQueryModel(this);
    m_table = new QTableView;
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(38);
    m_table->horizontalHeader()->setHighlightSections(false);

    auto *listW = new QWidget;
    auto *ll = new QVBoxLayout(listW);
    ll->setContentsMargins(0, 0, 0, 0);
    ll->setSpacing(12);
    ll->addLayout(ligne1);
    ll->addLayout(ligne2);
    ll->addWidget(m_table, 1);

    auto *h = new QHBoxLayout(page);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(18);
    h->addWidget(formCard);
    h->addWidget(makeCard(listW), 1);

    connect(btnAjouter, &QPushButton::clicked, this, &smartweather::ajouter);
    connect(btnModifier, &QPushButton::clicked, this, &smartweather::modifier);
    connect(btnSupprimer, &QPushButton::clicked, this, &smartweather::supprimer);
    connect(btnVider, &QPushButton::clicked, this, &smartweather::viderFormulaire);
    connect(btnCsv, &QPushButton::clicked, this, &smartweather::exporterCsv);
    connect(btnPdf, &QPushButton::clicked, this, &smartweather::exporterPdf);
    connect(btnStats, &QPushButton::clicked, this, &smartweather::afficherStats);
    connect(m_recherche, &QLineEdit::textChanged, this, &smartweather::charger);
    connect(m_critere, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &smartweather::charger);
    connect(m_tri, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &smartweather::charger);
    connect(m_table, &QTableView::clicked, this, &smartweather::remplirFormulaire);
    return page;
}

void smartweather::appliquerStyle()
{
    setStyleSheet(
        "QWidget { background:#F5F7FA; color:#37474F; font-family:Arial; font-size:11pt; }"
        "QLabel { background:transparent; }"
        "QFrame#card { background:white; border:none; border-radius:14px; }"
        "QFrame#card QWidget { background:white; }"
        "QFrame#card QCalendarWidget QWidget { background:white; }"

        "QFrame#hero { background:qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1E88E5, stop:1 #0D47A1); }"
        "QFrame#hero QLabel { background:transparent; color:white; }"

        "QFrame#sidebar { background:#37474F; }"
        "QFrame#sidebar QLabel { background:transparent; }"
        "QLabel#brand { color:white; font-size:16pt; font-weight:bold; padding-left:22px; }"
        "QLabel#brandSub { color:#90A4AE; font-size:9pt; padding-left:24px; }"
        "QPushButton#nav { background:transparent; color:#CFD8DC; text-align:left; padding:13px 22px;"
        "                  border:none; border-left:4px solid transparent; border-radius:0; font-weight:normal; }"
        "QPushButton#nav:hover { background:rgba(255,255,255,0.07); color:white; }"
        "QPushButton#nav:checked { background:rgba(30,136,229,0.30); color:white;"
        "                          border-left:4px solid #1E88E5; font-weight:bold; }"
        "QPushButton#nav:disabled { color:#78909C; }"
        "QPushButton#logout { background:transparent; color:#CFD8DC; border:1px solid #607D8B;"
        "                     border-radius:8px; padding:9px; font-weight:normal; }"
        "QPushButton#logout:hover { background:#E53935; border-color:#E53935; color:white; }"

        "QLabel#pageTitle { font-size:21pt; font-weight:bold; color:#37474F; }"
        "QLabel#cardTitle { font-size:12pt; font-weight:bold; color:#37474F; }"
        "QLabel#kpiValue { font-size:38pt; font-weight:bold; color:#1E88E5; }"
        "QLabel#kpiSub { color:#78909C; font-size:10pt; }"
        "QLabel#userNom { font-weight:bold; }"
        "QLabel#erreur { color:#E53935; }"
        "QLabel#avatar { background:#1E88E5; color:white; border-radius:20px; font-weight:bold; font-size:13pt; }"

        "QLineEdit, QComboBox, QDateEdit { background:white; border:1px solid #D5DEE8;"
        "                                  border-radius:8px; padding:8px 10px; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border:1px solid #1E88E5; }"
        "QLineEdit#globalSearch { border-radius:20px; padding:9px 18px; min-width:250px; border:1px solid #E3EAF2; }"
        "QLineEdit#loginUser, QLineEdit#loginPass { padding:12px 14px; }"

        "QPushButton { background:#1E88E5; color:white; border:none; border-radius:8px;"
        "              padding:9px 16px; font-weight:bold; }"
        "QPushButton:hover { background:#1565C0; }"
        "QPushButton:disabled { background:#B0BEC5; color:#ECEFF1; }"
        "QPushButton#danger { background:#E53935; }"
        "QPushButton#danger:hover { background:#C62828; }"
        "QPushButton#btnLogin { padding:13px; font-size:12pt; }"

        "QTableView { background:white; alternate-background-color:#F8FAFC; border:none;"
        "             selection-background-color:#DCEBFA; selection-color:#37474F; }"
        "QTableView::item { padding:4px 6px; border:none; }"
        "QHeaderView::section { background:white; color:#1E88E5; padding:10px 6px; border:none;"
        "                       border-bottom:2px solid #1E88E5; font-weight:bold; }"

        "QCalendarWidget QWidget#qt_calendar_navigationbar { background:#1E88E5; border-radius:8px; }"
        "QCalendarWidget QToolButton { color:white; background:transparent; border:none; font-weight:bold; }"
        "QCalendarWidget QSpinBox { color:white; background:transparent; border:none; }"
        "QCalendarWidget QAbstractItemView:enabled { color:#37474F; background:white;"
        "    selection-background-color:#1E88E5; selection-color:white; outline:0; }"
        "QCalendarWidget QAbstractItemView:disabled { color:#B0BEC5; }"

        "QScrollArea { border:none; background:transparent; }"
        "QScrollBar:vertical { background:transparent; width:10px; }"
        "QScrollBar::handle:vertical { background:#CFD8DC; border-radius:5px; min-height:30px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }");
}

void smartweather::erreur(const QString &msg)
{
    QMessageBox::warning(this, "Erreur", msg);
}

bool smartweather::formulaireValide()
{
    if (m_nom->text().trimmed().isEmpty() || m_prenom->text().trimmed().isEmpty()) {
        erreur("Le nom et le prénom sont obligatoires.");
        return false;
    }
    static const QRegularExpression re(R"(^[\w.+-]+@[\w-]+\.[\w.-]+$)");
    if (!re.match(m_email->text().trimmed()).hasMatch()) {
        erreur("Adresse email invalide.");
        return false;
    }
    if (m_gouvernorat->text().trimmed().isEmpty()) {
        erreur("Le gouvernorat est obligatoire.");
        return false;
    }
    return true;
}

void smartweather::ajouter()
{
    if (!formulaireValide()) return;
    QSqlQuery q;
    q.prepare("INSERT INTO employe (nom,prenom,email,poste,structure,gouvernorat,"
              "date_recrutement,statut,type_horaire) VALUES (?,?,?,?,?,?,?,?,?)");
    q.addBindValue(m_nom->text().trimmed());
    q.addBindValue(m_prenom->text().trimmed());
    q.addBindValue(m_email->text().trimmed());
    q.addBindValue(m_poste->currentText());
    q.addBindValue(m_structure->currentText());
    q.addBindValue(m_gouvernorat->text().trimmed());
    q.addBindValue(m_date->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_statut->currentText());
    q.addBindValue(m_horaire->currentText());
    if (!q.exec()) { erreur(q.lastError().text()); return; }
    viderFormulaire();
    charger();
}

void smartweather::modifier()
{
    if (m_idCourant < 0) { erreur("Sélectionnez un employé dans le tableau."); return; }
    if (!formulaireValide()) return;
    QSqlQuery q;
    q.prepare("UPDATE employe SET nom=?,prenom=?,email=?,poste=?,structure=?,gouvernorat=?,"
              "date_recrutement=?,statut=?,type_horaire=? WHERE id_employe=?");
    q.addBindValue(m_nom->text().trimmed());
    q.addBindValue(m_prenom->text().trimmed());
    q.addBindValue(m_email->text().trimmed());
    q.addBindValue(m_poste->currentText());
    q.addBindValue(m_structure->currentText());
    q.addBindValue(m_gouvernorat->text().trimmed());
    q.addBindValue(m_date->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_statut->currentText());
    q.addBindValue(m_horaire->currentText());
    q.addBindValue(m_idCourant);
    if (!q.exec()) { erreur(q.lastError().text()); return; }
    charger();
}

void smartweather::supprimer()
{
    if (m_idCourant < 0) { erreur("Sélectionnez un employé dans le tableau."); return; }
    if (QMessageBox::question(this, "Confirmation",
                              "Supprimer définitivement cet employé ?") != QMessageBox::Yes) return;
    QSqlQuery q;
    q.prepare("DELETE FROM employe WHERE id_employe=?");
    q.addBindValue(m_idCourant);
    if (!q.exec()) { erreur(q.lastError().text()); return; }
    viderFormulaire();
    charger();
}

void smartweather::charger()
{
    static const char *colRecherche[] = {"nom", "poste", "structure", "gouvernorat"};
    static const char *colTri[] = {"nom", "poste", "date_recrutement", "structure"};

    QString sql = "SELECT id_employe, nom, prenom, email, poste, structure, gouvernorat, "
                  "date_recrutement, statut, type_horaire FROM employe";
    const QString texte = m_recherche->text().trimmed();
    if (!texte.isEmpty())
        sql += QString(" WHERE %1 LIKE ?").arg(colRecherche[m_critere->currentIndex()]);
    sql += QString(" ORDER BY %1").arg(colTri[m_tri->currentIndex()]);

    QSqlQuery q;
    q.prepare(sql);
    if (!texte.isEmpty()) q.addBindValue("%" + texte + "%");
    q.exec();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_model->setQuery(std::move(q));
#else
    m_model->setQuery(q);
#endif

    const QStringList titres = {"ID", "Nom", "Prénom", "Email", "Poste", "Structure",
                                "Gouvernorat", "Recrutement", "Statut", "Horaire"};
    for (int i = 0; i < titres.size(); ++i)
        m_model->setHeaderData(i, Qt::Horizontal, titres[i]);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(C_ID, QHeaderView::ResizeToContents);
}

void smartweather::remplirFormulaire(const QModelIndex &index)
{
    const QSqlRecord r = m_model->record(index.row());
    m_idCourant = r.value(C_ID).toInt();
    m_nom->setText(r.value(C_NOM).toString());
    m_prenom->setText(r.value(C_PRENOM).toString());
    m_email->setText(r.value(C_EMAIL).toString());
    m_poste->setCurrentText(r.value(C_POSTE).toString());
    m_structure->setCurrentText(r.value(C_STRUCT).toString());
    m_gouvernorat->setText(r.value(C_GOUV).toString());
    m_date->setDate(QDate::fromString(r.value(C_DATE).toString(), "yyyy-MM-dd"));
    m_statut->setCurrentText(r.value(C_STATUT).toString());
    m_horaire->setCurrentText(r.value(C_HORAIRE).toString());
}

void smartweather::viderFormulaire()
{
    m_idCourant = -1;
    m_nom->clear(); m_prenom->clear(); m_email->clear(); m_gouvernorat->clear();
    m_poste->setCurrentIndex(0); m_structure->setCurrentIndex(0);
    m_statut->setCurrentIndex(0); m_horaire->setCurrentIndex(0);
    m_date->setDate(QDate::currentDate());
    m_table->clearSelection();
}

void smartweather::exporterCsv()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter", "employes.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) { erreur("Impossible d'écrire le fichier."); return; }
    QTextStream out(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out.setGenerateByteOrderMark(true);
    const int cols = m_model->columnCount();
    for (int c = 0; c < cols; ++c)
        out << m_model->headerData(c, Qt::Horizontal).toString() << (c + 1 < cols ? ";" : "\n");
    for (int r = 0; r < m_model->rowCount(); ++r)
        for (int c = 0; c < cols; ++c)
            out << m_model->data(m_model->index(r, c)).toString() << (c + 1 < cols ? ";" : "\n");
}

void smartweather::exporterPdf()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter", "employes.pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString html = "<h2 style='color:#1E88E5'>Liste des employés - INM</h2>"
                   "<table border='1' cellspacing='0' cellpadding='4' width='100%'><tr>";
    const int cols = m_model->columnCount();
    for (int c = 0; c < cols; ++c)
        html += "<th bgcolor='#1E88E5'><font color='white'>" +
                m_model->headerData(c, Qt::Horizontal).toString() + "</font></th>";
    html += "</tr>";
    for (int r = 0; r < m_model->rowCount(); ++r) {
        html += "<tr>";
        for (int c = 0; c < cols; ++c)
            html += "<td>" + m_model->data(m_model->index(r, c)).toString().toHtmlEscaped() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);
}

void smartweather::afficherStats()
{
    auto bloc = [](const QString &titre, const QString &col) {
        QString s = "<b>" + titre + "</b><br>";
        QSqlQuery q("SELECT " + col + ", COUNT(*) FROM employe GROUP BY " + col);
        while (q.next())
            s += q.value(0).toString() + " : " + q.value(1).toString() + "<br>";
        return s + "<br>";
    };
    QMessageBox::information(this, "Statistiques",
                             bloc("Par poste", "poste") + bloc("Par structure", "structure") +
                                 bloc("Par gouvernorat", "gouvernorat"));
}