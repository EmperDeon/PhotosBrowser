/*
	Copyright (c) 2017-2018 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include "Window.h"
#include <vendor/verdigris/src/wobjectimpl.h>
#include <utility>

Window::Window(QString dir) {
    provider = new ImageProvider(this, std::move(dir));

    m = new QMenu;
    m->addAction("Change \"Current\" dir", provider->io, &PIO::setNewCurrentDir);
    m->addAction("Change \"Deleted\" dir", provider->io, &PIO::setNewDeletedDir);
    m->addAction("Change \"GoodToSave\" dir", provider->io, &PIO::setNewSavedDir);
    m->addAction("Open current dir", provider->io, &PIO::openCurrentDir);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &Window::customContextMenuRequested,
            this, &Window::showMenu
    );
}

void Window::toggleFullScreen() {
    this->hide();
    if (isFullScreen) {
        this->showMaximized();

    } else {
        this->showFullScreen();
    }

    isFullScreen = !isFullScreen;
    provider->loadImage();
}

void Window::resetCoord() {
    tx = 0;
    ty = 0;
}

//
// Qt overrides
//

void Window::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        /* F12 */ case 16777274:
            /* Esc */ case 16777216:
            toggleFullScreen();
            break;
            /* Tab */ case 16777217:
            isText = !isText;
            update();
            break;
        default:
            provider->keyPressEvent(event->key());
            break;
    }
}

void Window::wheelEvent(QWheelEvent *event) {
    int ex = width() / 2 - event->x(), ey = height() / 2 - event->y();

    double inc = event->delta() > 0 ? scale * 1.25 : scale * 0.75;
    inc -= scale;

    float ox = (ex + tx) / scale, oy = (ey + ty) / scale;
    scale += inc;

    tx = int(ox * scale - ex);
    ty = int(oy * scale - ey);

    if (scale < original_scale) {
        scale = original_scale;
        tx = ty = 0;
    }

    update();
}

void Window::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(QBrush(QColor(56, 56, 56, 255)));
    p.drawRect(-1, -1, width() + 1, height() + 1);

    static QFont font_fan("PT Sans", 11);

    QImage image = provider->getOriginal();
    int x1, y1, x2, y2;

    if (image.width() != 0) {
        x2 = int(image.width() * scale) + 1;
        y2 = int(image.height() * scale) + 1;

        x1 = (width() - x2) / 2;
        y1 = (height() - y2) / 2;

        p.save();
        p.translate(tx, ty);

        p.drawImage(QRect(x1, y1, x2, y2), scale == original_scale ? provider->getScaled() : image);

        p.restore();
    }

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

    p.end();

    event->accept();
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    tx += event->x() - px;
    ty += event->y() - py;
    px = event->x();
    py = event->y();

    update();
}

void Window::mousePressEvent(QMouseEvent *event) {
    px = event->x();
    py = event->y();

//	if (event->button() == Qt::LeftButton) {
//		int w = width(), h = height();

//		if (py < (h/5*2)) {
//			provider->cop();

//		} else {
//			if (px < w/2) {
//				provider->prev();
//			} else {
//				provider->next();
//			}
//		}
//	}
}

void Window::closeEvent(QCloseEvent *) {
    provider->closeEvent();
}

void Window::reload() {
    QTimer::singleShot(100, [=]() {
        provider->loadImage();
        update();
    });
}

void Window::titleChanged(QString s) {
    setWindowTitle(s);
}

W_OBJECT_IMPL(Window)
