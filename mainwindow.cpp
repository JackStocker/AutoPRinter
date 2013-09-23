#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QIcon>

MainWindow::MainWindow ( QWidget *parent ) :
   QMainWindow ( parent ),
   ui ( new Ui::MainWindow )
{
   ui->setupUi ( this ) ;

   this->setWindowFlags ( Qt::Window |
                          Qt::CustomizeWindowHint |
                          Qt::WindowTitleHint |
                          Qt::WindowSystemMenuHint |
                          Qt::WindowMinimizeButtonHint ) ;

   SystemTray = new QSystemTrayIcon ( QIcon ( ":/Icons/AutoPRinter_icon.ico" ), this ) ;
   SystemTray->setToolTip( "AutoPRinter" ) ;
   SystemTray->show () ;

   connect ( SystemTray, SIGNAL ( activated ( QSystemTrayIcon::ActivationReason ) ),
                         SLOT ( system_tray_activated ( QSystemTrayIcon::ActivationReason ) ) ) ;

   QSettings settings ( ".\\settings.ini", QSettings::IniFormat ) ;
   WatchDir = settings.value ( "AutoPRint/Directory", "" ).toString () ;
   Printer  = settings.value ( "AutoPRint/Printer", "" ).toString () ;

   Watcher = new QFileSystemWatcher ( this ) ;
   connect ( Watcher, SIGNAL ( directoryChanged ( QString ) ), SLOT ( fileChanged ( QString ) ) ) ;
   connect ( Watcher, SIGNAL ( fileChanged ( QString ) ), SLOT ( directoryChanged ( QString ) ) ) ;

   ui->Le_Dir->setText ( WatchDir ) ;

   reConnect () ;

   QTimer::singleShot ( 10, this, SLOT ( HideWindow () ) ) ;
}

MainWindow::~MainWindow ()
{
   delete ui ;
}

void
MainWindow::
fileChanged ( QString str )
{
   QDir    dir ( str ) ;
   QString file ;
   bool    one_new_file = false ;

   FileList = dir.entryList ( QDir::Files ) ;

   for ( int x = 0 ; x < FileList.size () ; ++x )
   {
      file = WatchDir + FileList.at ( x ) ;

      if ( QFile::exists ( file ) )
      {
         one_new_file = true ;

         if ( !DeleteFileList.contains ( file ) )
         {
            ui->Lw_Files->clear () ;
            ui->Lw_Files->addItems ( DeleteFileList ) ;
            ui->Lw_Files->addItem ( file ) ;

            QProcess *soffice = new QProcess ( this ) ;
            connect ( soffice, SIGNAL ( finished ( int ) ), SLOT ( print_finished ( int ) ) ) ;

            QStringList args ;
            args << "--headless" << "-pt" << Printer << file ;

            soffice->start ( ".\\SOffice\\program\\soffice.exe", args ) ;
         }
      }
   }

   if ( one_new_file )
   {
      QTimer::singleShot ( 5000, this, SLOT ( deleteFile () ) ) ;
   }
}

void
MainWindow::
print_finished ( int exit_code )
{
   if ( QObject::sender () != 0 )
   {
      QProcess *soffice = static_cast < QProcess * > ( QObject::sender () ) ;

      if ( exit_code == QProcess::NormalExit )
      {
         QString file = soffice->arguments ().last () ;
         DeleteFileList << file ;

         SystemTray->showMessage ( "Printing" , file ) ;
      }
      else
      {
         ui->Lw_Files->clear () ;
         ui->Lw_Files->addItems ( DeleteFileList ) ;
      }
   }
}

void
MainWindow::
deleteFile ()
{
   foreach ( QString file, DeleteFileList )
   {
      if ( !QFile ( file ).exists () )
      {
         DeleteFileList.removeOne ( file ) ;
      }
      else
      {
         bool ret = QFile::remove ( file ) ;

         if ( ret )
         {
            DeleteFileList.removeOne ( file ) ;
         }
      }
   }

   ui->Lw_Files->clear () ;
   ui->Lw_Files->addItems ( DeleteFileList ) ;

   if ( DeleteFileList.size () > 0 )
   {
      QTimer::singleShot ( 5000, this, SLOT ( deleteFile () ) ) ;
   }
}

void
MainWindow::
directoryChanged ( QString str )
{
   if ( !QDir ( WatchDir ).exists () )
   {
      QTimer::singleShot ( 60000, this, SLOT ( reConnect () ) ) ;
   }
}

void
MainWindow::
reConnect ()
{
   if ( QDir ( WatchDir ).exists () )
   {
      Watcher->addPath ( WatchDir ) ;
   }
   else
   {
      QTimer::singleShot ( 60000, this, SLOT ( reConnect () ) ) ;
   }
}

void
MainWindow::
system_tray_activated ( QSystemTrayIcon::ActivationReason reason )
{
   switch ( reason )
   {
      case QSystemTrayIcon::DoubleClick:
      {
         this->show () ;
         this->raise () ;

         break ;
      }
      case QSystemTrayIcon::Trigger:
      case QSystemTrayIcon::MiddleClick:
      default:
         ;
   }
}

bool
MainWindow::
event ( QEvent *event )
{
   if ( ( event->type() == QEvent::WindowStateChange ) &&
        ( isMinimized () ) )
   {
      this->hide () ;
      return true ;
   }
   else
   {
      return QMainWindow::event ( event ) ;
   }
}

void
MainWindow::
HideWindow ()
{
   this->hide () ;
}
