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

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSizePolicy>
#include "core.h"

namespace pichi
{

MainWindow::MainWindow()
{
	inChatList = new QListWidget();
	inputLine = new QLineEdit();
	chatBox = new QTextBrowser();
	
	QHBoxLayout* main = new QHBoxLayout();
	QVBoxLayout* chat = new QVBoxLayout();
	chat->addWidget(chatBox);
	chat->addWidget(inputLine);
	main->addLayout(chat);
	main->addWidget(inChatList);
	setLayout(main);
	
	//растяжения
	//inChatList->setSizePolicy(QSizePolicy(1));
	//chatBox->setSizePolicy(3);
	//inputLine->setSizePolicy(3);
	
	connect(inputLine, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
}

QTextBrowser* MainWindow::getChatBox() const
{
	return chatBox;
}

QLineEdit* MainWindow::getInputLine() const
{
	return inputLine;
}

QListWidget* MainWindow::getList() const
{
	return inChatList;
}

void MainWindow::setCore(core* p)
{
	pichi = p;
}

void MainWindow::sendMessage()
{
	pichi->sendMessage(pichi->getDefaultRoom(), inputLine->text().toUtf8().data());
	inputLine->clear();
}

}