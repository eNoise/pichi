/*
    Pichi XMPP (Jabber) Bot.
    Copyright (C) 2010  Alexey Kasyanchuk (deg@uruchie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QTextBrowser>
#include <QString>

namespace pichi
{

class core;
  
class MainWindow : public QDialog
{
Q_OBJECT
public:
  MainWindow();
  void setCore(core* p);
  QListWidget* getList() const;
  QLineEdit* getInputLine() const;
  QTextBrowser* getChatBox() const;
private:
  QListWidget* inChatList;
  QLineEdit* inputLine;
  QTextBrowser* chatBox;
  
  core* pichi;
public slots:
  void sendMessage();
};

}

#endif // PICHIEXCEPTION_H
