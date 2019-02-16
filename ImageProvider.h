/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#ifndef PHOTOSBROWSER_IMAGEPROVIDER_H
#define PHOTOSBROWSER_IMAGEPROVIDER_H

#include <Pio.h>
#include <ImageLoader.h>
#include <GifLoader.h>
#include <Window.h>
#include <vendor/verdigris/src/wobjectdefs.h>


class Window;

class PIO;

class ImageLoader;

class GifLoader;

class ImageProvider : public QObject {
    W_OBJECT(ImageProvider)

    Window *wnd;

    ImageLoader *imgLoader;
    GifLoader *gifLoader;
    int loaderType = 0;

    QImage iOriginal, iScaled;
    QString text;
    QMutex *mutex;
    QJsonObject lastPicture;

    QStringList *files;
    int current = 0;

    inline QString getDir(QString f) {
        int r = f.lastIndexOf('/');

        return f.remove(r, f.length() - r);
    }

public: /* signals */
    void titleChanged(QString s) W_SIGNAL(titleChanged, s)

public: // Fields
    PIO *io;
    bool isLoading = false;

public: // Methods
    ImageProvider(Window *w, QString dir);

    void keyPressEvent(int i);

    void closeEvent();

    // Images loading/viewing
    void next(int i = 1);

    void prev(int i = 1);

    void loadImage(bool rel = false);

    void goTo();

    void toggleBlur();

    // IO
    bool isImg(QString s);

    void loadDir();

    void del();

    void cop();

    void exp();

    // Getters & Setters
    QImage &getOriginal() {
        QMutexLocker m(mutex);
        return iOriginal;
    }

    QImage &getScaled() {
        QMutexLocker m(mutex);
        return iScaled;
    }

    QString getText() const { return text; }

    int getCurrent() { return current; }

    int getSize() { return files->size(); }

    Window *getWnd() { return wnd; }

    void setOriginal(QImage &i);

    void setScaled(QImage &i);

    void setText(QString t);

    void setLoading(bool b);

    void setOriginalScale(float f);

    void setScale(float f);

    void saveLastPicture() const;

    void loadNewDir(int inc);
};

#endif //PHOTOSBROWSER_IMAGEPROVIDER_H
