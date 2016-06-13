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

PsWnd::PsWnd(QString c) {
	files = new QStringList;
	delHs = new QStringList;
	resize = new Resize;
 font_fan = QFont("Fantasque Sans Mono", 11);

	dirCurrent = "";
	dirDeleted = sett.value("deletedDir", "").toString();
	dirSave = sett.value("godnotaDir", "").toString();

	m = new QMenu;
	m->addAction("Change \"Current\" dir", this, SLOT(setCDir()));
	m->addAction("Change \"Deleted\" dir", this, SLOT(setDDir()));
	m->addAction("Change \"GoodToSave\" dir", this, SLOT(setGDir()));
	m->addAction("Open current dir", this, SLOT(openDir()));
//	m->addAction("Update image", this, SLOT(update()));
	g_mov = new QMovie;
	g_mov->stop();

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(
		                     const QPoint&)), this, SLOT(showMenu(
			                                                 const QPoint&)));
	connect(g_mov, SIGNAL(frameChanged(int)), this, SLOT(nextFrame(int)));
 c = "E:/Wallpapers/dwall/WUHD/105224.jpg";
	if (c == "") {
		setCDir();
	} else {
		dirCurrent = c;
		load();
	}
}

// Slots
void PsWnd::setCDir() {
	dirCurrent = QFileDialog::getExistingDirectory(this, "Select dir with files", dirCurrent) + '/';
	load();
}

void PsWnd::setDDir() {
	dirDeleted = QFileDialog::getExistingDirectory(this, "Select dir for deleted files", dirDeleted) + '/';
	sett.setValue("deletedDir", dirDeleted);
}

void PsWnd::setGDir() {
	dirSave = QFileDialog::getExistingDirectory(this, "Select dir for saved files", dirSave) + '/';
	sett.setValue("godnotaDir", dirSave);
}

void PsWnd::openDir() {
	QDesktopServices::openUrl(QUrl::fromLocalFile(dirCurrent));
}

void PsWnd::next(int i) {
	current = current < lSize - i ? current + i : 0;
	loadImage();
}

void PsWnd::prev(int i) {
	current = current >= i ? current - i : lSize - i;
	loadImage();
}

void PsWnd::del() {
	if (dirDeleted == "") setDDir();
	QString file = files->value(current);
	if (QFile(file).exists()) {
		file = file.remove(0, file.lastIndexOf('/') + 1);
		if (delHs->size() > 10) {
			delHs->removeFirst();
		}
		delHs->push_back(file);

		QFile::rename(dirCurrent + file, dirDeleted + file);
		files->removeAt(current);
	}
	current--;
	lSize--;

	next(1);
}

void PsWnd::cop() {
	if (dirSave == "") setGDir();
	QString file = files->value(current);
	if (QFile(file).exists()) {
		file = file.remove(0, file.lastIndexOf('/') + 1);

		QFile::rename(dirCurrent + file, dirSave + file);
		files->removeAt(current);
	}
	current--;
	lSize--;

	next(1);
}

void PsWnd::und() {
	if (!delHs->isEmpty()) {
		QString file = delHs->takeLast();
		QFile::rename(dirDeleted + file, dirCurrent + file);
		lSize++;
		current++;
	}
}
// Slots


// Image loading
bool PsWnd::isImg(QString s) {
	return s.endsWith(".png") || s.endsWith(".jpg") || s.endsWith(".gif");
}

void PsWnd::load() {
	files->clear();

	QString c = dirCurrent;
	c = c.replace("\\", "/");
	dirCurrent = isImg(c) ? QStringRef(&c, 0, c.lastIndexOf('/')).toString() : (c.endsWith('/') ? c : c + '/');

	if (isImg(c)) c = c.remove(0, c.lastIndexOf('/') + 1);

	for ( QFileInfo i : QDir(dirCurrent).entryInfoList({"*.png", "*.jpg", "*.gif"}, QDir::NoDotAndDotDot | QDir::Files)) {
		files->append(i.absolutePath() + '/' + i.fileName());
	}

	if (isImg(c)) {
		for ( int i = 0; i < files->size(); i++ )
			if (files->value(i, "").endsWith(c))
				current = i;
	} else {
		current = 0;
	}

	lSize = files->size();
	if (dirCurrent != "") loadImage();
}

void PsWnd::loadImage() {
	QString file = files->value(current);

	if (file.endsWith(".gif")) {
		oscale = scale = 1.0f;
		g_mov->stop();
		g_mov->setFileName(file);
		g_mov->start();

		title = QString("File: %1 | %2/%3 ");
		title = title.arg(file.remove(0, file.lastIndexOf('/') + 1))
			.arg(current + 1)
			.arg(this->lSize);

	} else {
		g_mov->stop();

		QImage in(file);
		image = r ? process(in) : in;

		simg = image.height() > height() || image.width() > width() ?
		       image.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) : image;

		float s1 = float(height()) / image.height(), s2 = float(width()) / image.width();
		scale = qMin(s1, s2);
		scale = scale > 1.0f ? 1.0f : scale;
		oscale = scale;

		QString rat, size = "%1 ";

		if (in.width() / 16 * 9 == in.height()) {
			rat = "16/9";
		} else if (in.width() / 16 * 10 == in.height()) {
			rat = "16/10";
		} else {
			rat = QString::number(float(in.width()) / in.height(), 'f', 3);
		}

		double s = QFile(file).size();
		if (s > 1024 * 1024) {
			s /= 1024 * 1024;
			size += " Mb";
		} else if (s > 1024) {
			s /= 1024;
			size += "Kb";
		} else {
			size += "b";
		}

		file = file.remove(0, file.lastIndexOf('/') + 1);

		if (in.width() != 0) {

			title = QString("File: %1 | Ratio: %2 (%3:%4) | Size: %5 | %6/%7 ");
			title = title.arg(file.size() < 100 ? file : QStringRef(&file, 0, 100).toString())
				.arg(rat)
				.arg(in.width())
				.arg(in.height())
				.arg(size.arg(s, 4, 'f', 3, ' '))
				.arg(current + 1)
				.arg(this->lSize);
		} else {
			title = QString("Error opening file: %1");
			title = title.arg(file);
		}
	}

	this->setWindowTitle(title);
	tx = 0;
	ty = 0;
	//scale = 1.0f;
	update();
}

void PsWnd::chFull() {
	this->hide();
	if (f) {
		this->showMaximized();

	} else {
		this->showFullScreen();

	}
	f = !f;
	loadImage();
}

void PsWnd::chBlur() {
	r = !r;
	loadImage();
}
// Image loading


// Qt overrides
void PsWnd::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
		case 16777234:
			prev(1);
			break;
		case 16777236:
			next(1);
			break;
		case 16777237:
			prev(100);
			break;
		case 16777235:
			next(100);
			break;
		case 16777223:
			del();
			break;
		case 32      :
			cop();
			break;
		case 16777274:
		case 16777216:
			chFull();
			break;
		case 66      :
			chBlur();
			break;
		case 69      :
			exportCurrent();
			break;
		case 71      :
		case 1055    :
			current = QInputDialog::getInt(this, "Go to", "Go to:", current, 0, lSize) - 1, loadImage();
		case 16777217:
			t = !t;
			break;
		case Qt::Key_Z:
			und();
			break;
		default:
			qDebug() << event->key();
			break;
	}
}

void PsWnd::wheelEvent(QWheelEvent *event) {
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
	if (first) loadImage(), first = false;

	QPainter p(this);

	int x1, y1, x2, y2;
	p.setBrush(QBrush(QColor(56, 56, 56, 255)));
	p.drawRect(-1, -1, width() + 1, height() + 1);

	x2 = int(image.width() * scale) + 1;
	y2 = int(image.height() * scale) + 1;

	x1 = (width() - x2) / 2;
	y1 = (height() - y2) / 2;

	p.save();
	p.translate(tx, ty);
	p.drawImage(QRect(x1, y1, x2, y2), scale == oscale ? simg : image);

	p.restore();

	if (f && t) {
		p.setFont(font_fan);
		p.setBrush(QBrush(QColor(0, 0, 0, 127)));
		p.setPen(Qt::NoPen);

		QFontMetrics m(font_fan);
		int x = m.width(title) + 10, y = m.height() / 2 + 17;

		p.drawRect((width() - x) / 2, 0, x, y);

		p.setPen(QPen(QBrush(QColor(255, 255, 255)), 100));
		p.drawText((width() - x) / 2, 0, x, y, Qt::AlignCenter, title);
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


// Gif
void PsWnd::nextFrame(int i) {
	Q_UNUSED(i)

	this->simg = g_mov->currentImage();

	if (simg.width() > width() || simg.height() > height()) {
		simg = simg.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation);

		float s1 = float(height()) / simg.height(), s2 = float(width()) / simg.width();
		scale = qMin(s1, s2);
		scale = scale > 1.0f ? 1.0f : scale;
		oscale = scale;
	}
	this->image = simg;

	update();
}

// Gif


void PsWnd::exportCurrent() {
	if (dirSave == "") setGDir();
	QString file = files->value(current);
	file = file.remove(0, file.lastIndexOf('/') + 1);
	image.save(dirSave + file);

	next(1);
}

QImage PsWnd::process(QImage in) {
	float s1 = float(height()) / in.height(), s2 = float(width()) / in.width();
	int mw, mh;
	if (s1 < s2) {
		mh = in.height() > height() ? in.height() : height();
		mw = int(float(width()) / height() * (mh));
	} else {
		mw = in.width() > width() ? in.width() : width();
		mh = int(float(height()) / width() * (mw));
	}
	return resize->process(in, mw, mh);
}


