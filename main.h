#ifndef PHOTOSBROWSER_MAIN_H
#define PHOTOSBROWSER_MAIN_H
#include <QtWidgets>

class PsWnd : public QMainWindow{
	Q_OBJECT

	QMenu* m;
	QPixmap image, simg;
	QColor back;
 int tx, ty, px, py;
	float scale, oscale;

	QString dirDeleted, dirCurrent, dirGodnota, title;

	QStringList* files;
	QStringList* delHs;
 int lSize, current;

	bool f = false, t = false, r = false;

public slots:
	void showMenu(const QPoint& p){ m->popup(p);}
	void setCDir();
	void setDDir();
	void setGDir();
	void openDir();

	void next();
	void prev();
	void next(int i);
	void prev(int i);
	void del();
	void cop();
 void und();

	void load();
	void loadImage();
	void chFull();
	void chBlur();
public:
	PsWnd();


	QColor getBackColor(QImage c);
	QColor getWColor(QImage o, int i, int i1);
	QColor getHColor(QImage o, int i, int i1);
	QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0);
	QPixmap process(QPixmap in, int MW, int MH);

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	virtual void wheelEvent(QWheelEvent *qWheelEvent) override;
	virtual void paintEvent(QPaintEvent *qPaintEvent) override;
	virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
};

#endif //PHOTOSBROWSER_MAIN_H
