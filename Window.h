/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#ifndef PHOTOSBROWSER_WINDOW_H
#define PHOTOSBROWSER_WINDOW_H

#include <QtWidgets>
#include <ImageProvider.h>
#include <vendor/verdigris/src/wobjectdefs.h>


class ImageProvider;

class Window : public QMainWindow {
    W_OBJECT(Window)

    ImageProvider *provider;

    QMenu *m;
    int tx, ty, px, py;
    float scale, original_scale = 0.05f;

private:
    bool isFullScreen = false, isText = false;

private:
    void showMenu(const QPoint &p) { m->popup(p); }

    void toggleFullScreen();

    void titleChanged(QString s);

public:
    explicit Window(QString dir = "");

    void resetCoord();

    void reload();

protected:
    void keyPressEvent(QKeyEvent *keyEvent) override;

    void wheelEvent(QWheelEvent *qWheelEvent) override;

    void paintEvent(QPaintEvent *qPaintEvent) override;

    void mouseMoveEvent(QMouseEvent *mouseEvent) override;

    void mousePressEvent(QMouseEvent *mouseEvent) override;

    void closeEvent(QCloseEvent *event) override;

    friend class ImageProvider;
};


#endif //PHOTOSBROWSER_WINDOW_H
