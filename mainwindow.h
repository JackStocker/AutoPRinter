#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemWatcher>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QTimer>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();

protected:
   bool event ( QEvent *event ) ;

public slots:
   void fileChanged ( QString ) ;
   void deleteFile () ;
   void directoryChanged ( QString ) ;
   void reConnect () ;
   void system_tray_activated ( QSystemTrayIcon::ActivationReason ) ;
   void HideWindow () ;
   void print_finished ( int ) ;

private:
   Ui::MainWindow *ui ;

   QFileSystemWatcher *Watcher ;
   QSystemTrayIcon *SystemTray ;

   QStringList DeleteFileList ;
   QStringList FileList ;

   QString WatchDir ;
   QString Printer ;
} ;

#endif // MAINWINDOW_H
