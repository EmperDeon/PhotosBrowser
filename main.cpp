#include "main.h"


void qtMessageHandler(QtMsgType type, const QMessageLogContext &cont, const QString &msg) {
	Q_UNUSED(cont)
	switch (type) {
		case QtDebugMsg:
			printf("%s \n", msg.toStdString().c_str());
			break;// DBG:
		case QtWarningMsg:
			break;// WRN: printf("%s \n", msg.toStdString().c_str());
		case QtCriticalMsg:
			break;// CRT: printf("%s \n", msg.toStdString().c_str());
		case QtFatalMsg:
			break;// FTL: printf("%s \n", msg.toStdString().c_str());
	}
}

int main(int argv, char **argc) {
	QApplication a(argv, argc);
	qInstallMessageHandler(qtMessageHandler);
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

	dirCurrent = "";
	dirDeleted = sett.value("deletedDir", "").toString();
	dirGodnota = sett.value("godnotaDir", "").toString();

	m = new QMenu;
	m->addAction("Change \"Current\" dir", this, SLOT(setCDir()));
	m->addAction("Change \"Deleted\" dir", this, SLOT(setDDir()));
	m->addAction("Change \"GoodToSave\" dir", this, SLOT(setGDir()));
	m->addAction("Open current dir", this, SLOT(openDir()));
	m->addAction("Update image", this, SLOT(update()));
	g_mov = new QMovie;
	g_mov->stop();

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(
		                     const QPoint&)), this, SLOT(showMenu(
			                                                 const QPoint&)));
	connect(g_mov, SIGNAL(frameChanged(int)), this, SLOT(nextFrame(int)));

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
	dirGodnota = QFileDialog::getExistingDirectory(this, "Select dir for saved files", dirGodnota) + '/';
	sett.setValue("godnotaDir", dirGodnota);
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
	if (dirGodnota == "") setGDir();
	QString file = files->value(current);
	if (QFile(file).exists()) {
		file = file.remove(0, file.lastIndexOf('/') + 1);

		QFile::rename(dirCurrent + file, dirGodnota + file);
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
		isGif = true;
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
		isGif = false;

		QPixmap in(file), p;
		float s1 = float(height()) / in.height(), s2 = float(width()) / in.width();

		if (r) {
			int mw, mh;
			if (s1 < s2) {
				mh = in.height() > height() ? in.height() : height();
				mw = int(float(width()) / height() * (mh));
			} else {
				mw = in.width() > width() ? in.width() : width();
				mh = int(float(height()) / width() * (mw));
			}
			p = process(in, mw + 200, mh + 200);
		} else {
			p = in;
		}

		this->image = p;
		//	back = getBackColor(p.toImage());

		simg = p.height() > height() || p.width() > width() ?
		       p.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) : p;

		s1 = float(height()) / p.height(), s2 = float(width()) / p.width();
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
//		this->setCursor(QCursor(Qt::ArrowCursor));
		this->showMaximized();
		f = false;
	} else {
//		this->setCursor(QCursor(QPixmap(1, 1)));
		this->showFullScreen();
		f = true;
	}
	loadImage();
}

void PsWnd::chBlur() {
	r = !r;
	loadImage();
}

void PsWnd::chShad() {
	s = !s;
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
		case 78      :
			chShad();
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
	p.drawPixmap(x1, y1, x2, y2, scale == oscale ? simg : image);

	p.restore();

	if (f && t) {
		p.setFont(QFont("Fantasque Sans Mono", 11));
		p.setBrush(QBrush(QColor(0, 0, 0, 127)));
		p.setPen(Qt::NoPen);

		QFontMetrics m(QFont("Fantasque Sans Mono", 11));
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

	this->simg = g_mov->currentPixmap();

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


// Process
#define BLUR_COUNT 10

QColor PsWnd::getWColor(QImage o, int i, int i1) {
	int r = 0, g = 0, b = 0;
	int stx, enx, sty, eny, pc = 0;
	if (i <= 0 || i >= o.width()) return QColor();

	stx = i - 8;
	enx = i + 8;
	sty = (i1 == 0 ? 0 : o.height() - BLUR_COUNT);
	eny = (i1 == 0 ? BLUR_COUNT : o.height());

	for ( int tx = stx; tx < enx; tx++ )
		for ( int ty = sty; ty < eny; ty++ )
			if (tx >= 0 && tx < o.width() && ty >= 0 && ty < o.height()) {
				QColor c = o.pixel(tx, ty);
				r += c.red();
				g += c.green();
				b += c.blue();
				pc++;
			}

	return QColor(r / pc, g / pc, b / pc);
}

QColor PsWnd::getHColor(QImage o, int i, int i1) {
	int r = 0, g = 0, b = 0;
	int stx, enx, sty, eny, pc = 0;
	if (i < 0 || i >= o.height()) return QColor();

	stx = (i1 == 0 ? 0 : o.width() - BLUR_COUNT);
	enx = (i1 == 0 ? BLUR_COUNT : o.width());
	sty = i - 8;
	eny = i + 8;

	for ( int tx = stx; tx < enx; tx++ )
		for ( int ty = sty; ty < eny; ty++ )
			if (tx >= 0 && tx < o.width() && ty >= 0 && ty < o.height()) {
				QColor c = o.pixel(tx, ty);
				r += c.red();
				g += c.green();
				b += c.blue();
				pc++;
			}

	return QColor(r / pc, g / pc, b / pc);
}

bool PsWnd::isVColor(QImage o) {
	int tx = o.width() - 1;
	QRgb r1 = o.pixel(0, 0), r2 = o.pixel(tx, 0);

	for ( int ty = 0; ty < o.height(); ty++ )
		if (r1 != o.pixel(0, ty) || r2 != o.pixel(tx, ty))
			return false;

	return true;
}

bool PsWnd::isHColor(QImage o) {
	int ty = o.height() - 1;
	QRgb r1 = o.pixel(0, 0), r2 = o.pixel(0, ty);

	for ( int tx = 0; tx < o.height(); tx++ )
		if (r1 != o.pixel(tx, 0) || r2 != o.pixel(tx, ty))
			return false;

	return true;
}

QImage PsWnd::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent) {
	if (src.isNull()) return QImage();   //No need to do anything else!
	if (!effect) return src;             //No need to do anything else!
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(src));
	item.setGraphicsEffect(effect);
	scene.addItem(&item);
	QImage res(src.size() + QSize(extent * 2, extent * 2), QImage::Format_ARGB32);
	res.fill(Qt::transparent);
	QPainter ptr(&res);
	scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent * 2, src.height() + extent * 2));
	return res;
}

QPixmap PsWnd::process(QPixmap in, int MW, int MH) {
	if (in.isNull()) return in;
	if (in.width() + 200 == MW && in.height() + 200 == MH) return in;

	QImage o = in.toImage();
	QPixmap n = QPixmap(MW, MH);
	QPainter out(&n);
	bool shadow = true;

	// Fill background before blur
	int fW = MW / 2, fH = MH / 2;
	out.setBrush(QBrush(QColor(o.pixel(0, 0))));
	out.drawRect(0, 0, fW, fH);

	out.setBrush(QBrush(QColor(o.pixel(o.width() - 1, 0))));
	out.drawRect(fW, 0, fW, fH);

	out.setBrush(QBrush(QColor(o.pixel(0, o.height() - 1))));
	out.drawRect(0, fH, fW, fH);

	out.setBrush(QBrush(QColor(o.pixel(o.width() - 1, o.height() - 1))));
	out.drawRect(fW, fH, fW, fH);

	// 'Gradients'
	if (in.height() == MH - 200) {// Vertical-only
		shadow = drawGradV(out, o, MW);

	} else if (in.width() == MW - 200) {// Horizontal-only
		shadow = drawGradH(out, o, MH);

	} else {// Vertical, Horizontal and Corners
		drawGradC(out, o, MW, MH);

	}

	// Blur
	QGraphicsBlurEffect *eb = new QGraphicsBlurEffect;
	eb->setBlurRadius(20);
	eb->setBlurHints(QGraphicsBlurEffect::QualityHint);
	out.end();

	n = QPixmap::fromImage(applyEffectToImage(n.toImage(), eb));

	out.begin(&n);


	// Shadow
	if (shadow) {
		QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
		e->setBlurRadius(100);
		e->setOffset(-1, -1);
		e->setColor(Qt::black);
		o = applyEffectToImage(o, e, 150);
	}


	// Original
	int topX = (MW - o.width()) / 2, topY = (MH - o.height()) / 2;
	out.drawImage(topX, topY, o);
	// Original

	out.end();

	return n.copy(100, 100, MW - 200, MH - 200);
}
// Process

bool PsWnd::drawGradV(QPainter &out, QImage o, int MW) {
	int lS = (MW - o.width()) / 2, // Line Start
		lE = lS + o.width(), // Line End
		lW = 2, // Line width
		ti; // Temp int

	out.setPen(Qt::NoPen);
	out.drawImage(lS, 100, o);


	if (isVColor(o)) {
		out.setBrush(QBrush(QColor(o.pixel(0, 0))));
		out.drawRect(0, 0, lS, o.height() + 200);

		out.setBrush(QBrush(QColor(o.pixel(o.width() - 1, 0))));
		out.drawRect(lE, 0, MW, o.height() + 200);

		return false;

	} else {
		for ( int i = 0; i < o.height(); i++ ) {// Left - Right
			ti = i + 100;

			out.setPen(QPen(QBrush(getHColor(o, i, 0)), lW));
			out.drawLine(0, ti, lS, ti);

			out.setPen(QPen(QBrush(getHColor(o, i, 1)), lW));
			out.drawLine(lE, ti, MW, ti);
		}

		return true;
	}
}

bool PsWnd::drawGradH(QPainter &out, QImage o, int MH) {
	int lS = (MH - o.height()) / 2, // Line Start
		lE = lS + o.height(), // Line End
		lW = 2, // Line width
		ti; // Temp int

	out.setPen(Qt::NoPen);
	out.drawImage(100, lS, o);


	if (isHColor(o)) {
		out.setBrush(QBrush(QColor(o.pixel(0, 0))));
		out.drawRect(0, 0, o.width() + 200, lS);

		out.setBrush(QBrush(QColor(o.pixel(o.width() - 1, 0))));
		out.drawRect(0, lE, o.width() + 200, MH);

		return false;

	} else {
		for ( int i = 0; i < o.height(); i++ ) {// Left - Right
			ti = i + 100;

			out.setPen(QPen(QBrush(getWColor(o, i, 0)), lW));
			out.drawLine(ti, 0, ti, lS);

			out.setPen(QPen(QBrush(getWColor(o, i, 1)), lW));
			out.drawLine(ti, lE, ti, MH);
		}

		return true;
	}
}

void PsWnd::drawGradC(QPainter &out, QImage o, int MW, int MH) {
	int iW = o.width(), iH = o.height(),
  xS = (MW - iW) / 2, yS = (MH - iH) / 2,
		xE = xS + iW, yE = yS + iH,
		ti, pi;

	out.setPen(Qt::NoPen);
	out.drawImage(xS, yS, o);

	// Gradients Center
	int lW = 2;

	for ( int i = xS; i < iW - xS; i++ ) {// Top - Bottom
		ti = xS + i;

		out.setPen(QPen(QBrush(getWColor(o, i, 0)), lW));
		out.drawLine(ti, 0, ti, yS);

		out.setPen(QPen(QBrush(getWColor(o, i, 1)), lW));
		out.drawLine(ti, yE, ti, MH);
	}

	for ( int i = yS; i < iH - yS; i++ ) {// Left - Right
		ti = yS + i;

		out.setPen(QPen(QBrush(getHColor(o, i, 0)), lW));
		out.drawLine(0, ti, xS, ti);

		out.setPen(QPen(QBrush(getHColor(o, i, 1)), lW));
		out.drawLine(xE, ti, MW, ti);
	}
	// Gradient center


	// Gradient angles
	for ( int i = 0; i < xS; i++ ) {
		ti = xS + i;
  pi = i*2;

		out.setPen(QPen(QBrush(getWColor(o, i, 0)), lW));
		out.drawLine(pi, 0, ti, yS);

		out.setPen(QPen(QBrush(getWColor(o, iW - ti, 0)), lW));
		out.drawLine(iW + pi, 0, iW + i, yS);

		out.setPen(QPen(QBrush(getWColor(o, i, 1)), lW));
		out.drawLine(pi, MH - 1, ti, yE);

		out.setPen(QPen(QBrush(getWColor(o, iW - ti, 1)), lW));
		out.drawLine(iW + pi, MH - 1, iW + i, yE);
	}

	for ( int i = 0; i < yS; i++ ) {
		ti = yS + i;
  pi = i * 2;

		out.setPen(QPen(QBrush(getHColor(o, i, 0)), lW));
		out.drawLine(0, pi, xS, ti);

		out.setPen(QPen(QBrush(getHColor(o, iH - ti, 0)), lW));
		out.drawLine(0, iH + pi, xS, iH + i);

		out.setPen(QPen(QBrush(getHColor(o, i, 1)), lW));
		out.drawLine(MW - 1, pi, xE, ti);

		out.setPen(QPen(QBrush(getHColor(o, iH - ti, 1)), lW));
		out.drawLine(MW - 1, iH + pi, xE, iH + i);
	}
}

void PsWnd::exportCurrent() {
	if (dirGodnota == "") setGDir();
	QString file = files->value(current);
	file = file.remove(0, file.lastIndexOf('/') + 1);
	image.save(dirGodnota + file);
qDebug() << dirGodnota + file;
	next(1);
}
