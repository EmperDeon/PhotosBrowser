#include "main.h"

void qtMessageHandler(QtMsgType type, const QMessageLogContext &cont, const QString &msg) {
	Q_UNUSED(cont)
	switch (type) {
		case QtDebugMsg:
			printf("%s \n", msg.toStdString().c_str());
			break;
		default:;
	}
}

int main(int argv, char **argc) {
	QApplication a(argv, argc);
	qInstallMessageHandler(qtMessageHandler);
	QFontDatabase::addApplicationFont(":/FantasqueSansMono-Regular.ttf");
	QCoreApplication::setOrganizationName("IlzSoft");
	QCoreApplication::setOrganizationDomain("github.com/ilz2010");
	QCoreApplication::setApplicationName("PhotosBrowser");

	QStringList l = a.arguments();
	PsWnd w(l.size() == 2 ? l[1] : "");
	w.showMaximized();

	return a.exec();
}

PsWnd::PsWnd(QString dir) {
	provider = new ImageProvider(this, dir);
	loadingTimer = new QTimer();

	m = new QMenu;
	m->addAction("Change \"Current\" dir",    provider->io, SLOT(setNewCurrentDir()));
	m->addAction("Change \"Deleted\" dir",    provider->io, SLOT(setNewDeletedDir()));
	m->addAction("Change \"GoodToSave\" dir", provider->io, SLOT(setNewSavedDir()));
	m->addAction("Open current dir",          provider->io, SLOT(openCurrentDir()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	        this, SLOT(showMenu(const QPoint&))
	);

	connect(loadingTimer, &QTimer::timeout, [=](){
		loadingAngle += 8;
		loadingAngle = loadingAngle >= 360 ? 0 : loadingAngle;
		update();
	});

 loadingTimer->start(16);
}

void PsWnd::toggleFullScreen() {
	this->hide();
	if (isFullScreen) {
		this->showMaximized();

	} else {
		this->showFullScreen();

	}
	isFullScreen = !isFullScreen;
	provider->loadImage();
}

void PsWnd::resetCoord() {
	tx = 0;
	ty = 0;
}


// Qt overrides
void PsWnd::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
		case 16777274:
		case 16777216: toggleFullScreen(); break;
		case 16777217: isText = !isText; update(); break;
		default: provider->keyPressEvent(event->key()); break;
	}
}

void PsWnd::wheelEvent(QWheelEvent *event) {
	// TODO: Normal scale
	if (event->angleDelta().y() > 0) {
		scale += 0.5f;
	} else if (scale > oscale + 0.01f) {
		scale -= 0.5f;
	} else {
		scale = oscale;
		tx = 0;
		ty = 0;
	}
	this->update();
}

void PsWnd::paintEvent(QPaintEvent *event) {
	static bool first = true;
	if (first) provider->loadImage(), first = false;

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QBrush(QColor(56, 56, 56, 255)));
	p.drawRect(-1, -1, width() + 1, height() + 1);

	static QFont font_fan("Fantasque Sans Mono", 11);

	QImage image = provider->getOriginal();
	int x1, y1, x2, y2;

	if(image.width() != 0) {
		x2 = int(image.width() * scale) + 1;
		y2 = int(image.height() * scale) + 1;

		x1 = (width() - x2) / 2;
		y1 = (height() - y2) / 2;

		p.save();
		p.translate(tx, ty);
		p.drawImage(QRect(x1, y1, x2, y2), scale == oscale ? provider->getScaled() : image);

		p.restore();

		if (isFullScreen && isText) {
			p.setFont(font_fan);
			p.setBrush(QBrush(QColor(0, 0, 0, 127)));
			p.setPen(Qt::NoPen);

			QFontMetrics m(font_fan);
			int x = m.width(provider->getText()) + 10, y = m.height() / 2 + 17;

			p.drawRect((width() - x) / 2, 0, x, y);

			p.setPen(QPen(QBrush(QColor(255, 255, 255)), 100));
			p.drawText((width() - x) / 2, 0, x, y, Qt::AlignCenter, provider->getText());
		}

	}

	if(provider->isLoading){
		p.setPen(Qt::NoPen);
		p.setBrush(QBrush(QColor(40, 40, 40)));
  p.drawRoundedRect(QRect(width()-150, 10, 140, 50), 25, 25);

		p.setPen(QPen(QBrush(QColor(200, 200, 200, 150)), 3));
  p.drawArc(QRect(width()-140, 20, 30, 30), loadingAngle * 16, 4640);

		p.setFont(font_fan);
		p.setPen(QPen(QBrush(QColor(200, 200, 200, 225)), 3));
		p.drawText(width() - 100, 40, "Loading...");
 }

	p.end();

	event->accept();
}

void PsWnd::mouseMoveEvent(QMouseEvent *event) {
	tx += event->x() - px;
	ty += event->y() - py;
	px = event->x();
	py = event->y();
	update();
}

void PsWnd::mousePressEvent(QMouseEvent *event) {
	px = event->x();
	py = event->y();
}

void PsWnd::closeEvent(QCloseEvent *event) {
	Q_UNUSED(event);

	provider->closeEvent();
}