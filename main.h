#ifndef PHOTOSBROWSER_MAIN_H
#define PHOTOSBROWSER_MAIN_H
#include <QtWidgets>

class PsWnd : public QMainWindow{
	Q_OBJECT

	QMenu* m;
	QPixmap image, simg;
 int tx, ty, px, py;
	float scale, oscale;

	QString dirDeleted, dirCurrent, dirGodnota;

	QStringList* files;
	QStringList* delHs;
 int lSize, current;

public slots:
	void showMenu(const QPoint& p){ m->popup(p);}
	void setCDir();
	void setDDir();
	void setGDir();
	void openDir();

	void next();
	void prev();
	void del();
	void cop();
 void und();

	void load();
	void loadImage();

public:
	PsWnd();

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	virtual void wheelEvent(QWheelEvent *qWheelEvent) override;
	virtual void paintEvent(QPaintEvent *qPaintEvent) override;
	virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
};

#endif //PHOTOSBROWSER_MAIN_H
