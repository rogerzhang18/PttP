#include "PttP.h"

using namespace std;

PttP::PttP(QWidget *parent)
	: QMainWindow(parent)
	, mIOThread(new IOThread(this))
{
	ui.setupUi(this);

	populatePortMenu();

	// Menu option exit
	connect(ui.actionExit, &QAction::triggered, this, &QWidget::close, Qt::QueuedConnection);

	// Selecting a file
	connect(ui.pushButtonSelect, &QPushButton::pressed, mIOThread->GetFileManip(), &FileManip::SelectFile);
	connect(mIOThread->GetFileManip(), &FileManip::fileChanged, this, &PttP::SetFileName);

	// Start button to send ENQ
	connect(ui.pushButtonStart, &QPushButton::pressed, mIOThread, &IOThread::SendFile, Qt::QueuedConnection);

	//send RVI signal
	connect(ui.pushButtonStop, &QPushButton::pressed, mIOThread, &IOThread::SetRVI, Qt::QueuedConnection);

	// Display data from valid data frame
	connect(mIOThread, &IOThread::DataReceieved, this, &PttP::DisplayDataFromPort);

	// Updates the label on UI
	connect(mIOThread, SIGNAL(UpdateLabel(QString)), this, SLOT(UpdateLabel(QString)));

	mIOThread->start();
}

/*-------------------------------------------------------------------------------------------------
-- FUNCTION: populatePortMenu()
--
-- DATE: November 29, 2017
--
-- REVISIONS: N/A
--
-- DESIGNER: Benny Wang
--
-- PROGRAMMER: Benny Wang
--
-- INTERFACE: void populatePortMenu ()
--
-- RETURNS: void.
--
-- NOTES:
-- Populates the port menu in the menu bar with a list of all available serial ports on the system.
-------------------------------------------------------------------------------------------------*/
void PttP::populatePortMenu()
{
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	if (ports.size() == 0)
	{
		ui.menuPorts->setEnabled(false);
		return;
	}

	QAction* action;

	for (int i = 0; i < ports.size(); i++)
	{
		action = new QAction(this);
		action->setObjectName(ports[i].portName());
		action->setText(ports[i].portName());

		ui.menuPorts->addAction(action);

		connect(action, &QAction::triggered, mIOThread, &IOThread::SetPort);
	}
}

/*-------------------------------------------------------------------------------------------------
-- FUNCTION: SetFileName()
--
-- DATE: November 29, 2017
--
-- REVISIONS: N/A
--
-- DESIGNER: Benny Wang
--
-- PROGRAMMER: Benny Wang
--
-- INTERFACE: void SetFileName (const string)
--		const string newFileName: The full path and name of the new file.
--
-- RETURNS: void.
--
-- NOTES:
-- Populates the file name label with the full path and name of the new file.
-------------------------------------------------------------------------------------------------*/
void PttP::SetFileName(const string newFileName)
{
	ui.labelSelectedFile->setText(QString(newFileName.c_str()));
}

/*-------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayDataFromPort()
--
-- DATE: November 30, 2017
--
-- REVISIONS: N/A
--
-- DESIGNER: Benny Wang
--
-- PROGRAMMER: Benny Wang
--
-- INTERFACE: void DisplayDataFromPort (string data)
--		string data: The data that was unpacked from a valid data frame.
--
-- RETURNS: void.
--
-- NOTES:
-- This is a Qt Slot.
-- When a valid data frame is received and the data is extracted, the function extracting the data
-- will emit a signal containing the std::string representation of that data and this function will
-- take that data and display it.
-------------------------------------------------------------------------------------------------*/
void PttP::DisplayDataFromPort(const QString data)
{
	QPlainTextEdit* textEdit = ui.plainTextEdit;
	QScrollBar* scrollBar = textEdit->verticalScrollBar();

	textEdit->insertPlainText(data);
	scrollBar->setValue(scrollBar->maximum());
}

/*-------------------------------------------------------------------------------------------------
-- FUNCTION: UpdateLabel()
--
-- DATE: Dec 6, 2017
--
-- REVISIONS: N/A
--
-- DESIGNER: Roger Zhang
--
-- PROGRAMMER: Roger Zhang
--
-- INTERFACE: void UpdateLabel (string text)
--		string text: The text indicate what to update the UI.
--
-- RETURNS: void.
--
-- NOTES:
-- This is a Qt Slot.
-- When a valid data frame is received and the data is extracted, the function extracting the data
-- will emit a signal containing the std::string representation of that data and this function will
-- take that data and display it.
-------------------------------------------------------------------------------------------------*/

void PttP::UpdateLabel(const QString text)
{
	if (text == "ACK")
	{
		numACK += 1;
		ui.labelNumOfAcks->setText("Number of ACKs: " + QString::number(numACK));
	}
	else if (text == "PacketReceived")
	{
		numPackets += 1;
		ui.labelPacketsTransfered->setText("Packets Transfered: " + QString::number(numPackets));
	}
	// Error bit rate
	else
	{
		ui.labelBRE->setText("Bit Error Rate: " + text + "%");
	}
}