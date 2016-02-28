#include "wnd.h"

PsWnd::PsWnd() {
 loadDir();
}

void PsWnd::keyPressEvent(QKeyEvent *event) {

}

void PsWnd::setCDir() {
	dirCurrent = QFileDialog::getExistingDirectory(this, "Select dir for deleted files", dirCurrent);
	load();
}

void PsWnd::setDDir() {
	dirDeleted = QFileDialog::getExistingDirectory(this, "Select dir for deleted files", dirDeleted);
}

void PsWnd::load() {
	qDebug() << dirCurrent;

 for(QFileInfo f : QDir(dirCurrent).entryInfoList({"*.png", "*.jpg", "*.gif"}, QDir::NoDotAndDotDot | QDir::Files)){
	 files->append(f.absolutePath());
 }

	current = 0;
}
