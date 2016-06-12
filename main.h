#ifndef PHOTOSBROWSER_MAIN_H
#define PHOTOSBROWSER_MAIN_H
#include <QtWidgets>

class PsWnd : public QMainWindow{
	Q_OBJECT

	QMenu* m;
	QPixmap image, simg;
 int tx, ty, px, py;
	float scale, oscale;

	QSettings sett;
	QString dirDeleted, dirCurrent, dirGodnota, title;

	QStringList* files;
	QStringList* delHs;
 int lSize, current;

	bool f = false, t = false, r = true, s = true;
 // Gif
	QMovie* g_mov;
	bool isGif;

	bool isImg(QString s);

private slots:
	void nextFrame(int i);

public slots:
	void showMenu(const QPoint& p){ m->popup(p);}
	void setCDir();
	void setDDir();
	void setGDir();
	void openDir();

	void next(int i);
	void prev(int i);
	void del();
	void cop();
 void und();

	void load();
	void loadImage();
	void chFull();
	void chBlur();
	void chShad();

public:
	PsWnd(QString c = "");


	QColor getWColor(QImage o, int i, int i1);
	QColor getHColor(QImage o, int i, int i1);
	QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0);
	QPixmap process(QPixmap in, int MW, int MH);

 bool drawGradV(QPainter &out, QImage o, int MW);
	bool drawGradH(QPainter &out, QImage o, int MH);
	void drawGradC(QPainter &out, QImage o, int MW, int MH);

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	virtual void wheelEvent(QWheelEvent *qWheelEvent) override;
	virtual void paintEvent(QPaintEvent *qPaintEvent) override;
	virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
	bool isHColor(QImage o);
	bool isVColor(QImage o);
	void exportCurrent();
};

#endif //PHOTOSBROWSER_MAIN_H
