///////////////////////////////////////////////////////////////////////////////
// textprinter.h
// -------------------
// Copyright (c) 2007 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
///////////////////////////////////////////////////////////////////////////////


#include "textprinter.h"


TextPrinter::TextPrinter(QObject *parent)
    : QObject(parent), parent_(Q_NULLPTR),
                        printer_(new QPrinter(QPrinter::HighResolution)),
                        tempdoc_(Q_NULLPTR),
                        leftmargin_(15), rightmargin_(15), topmargin_(10), bottommargin_(10), spacing_(5),
                        headersize_(0.0), headerrule_(0.5), headertext_(QString()), footersize_(0.0), footerrule_(0.5), footertext_(QString()),
                        dateformat_(), duplex_(QPrinter::DuplexAuto)
{
    if (parent)
        parent_ = qobject_cast<QWidget*>(parent);

    printer_->setFullPage(true);
    printer_->setOrientation(QPrinter::Portrait);
    
    // for convenience, default to US_Letter for C/US/Canada
    // NOTE: bug in Qt, this value is not loaded by QPrintDialog
    switch (QLocale::system().country()) {
      //case QLocale::AnyCountry:
      case QLocale::Canada:
      case QLocale::UnitedStates:
      case QLocale::UnitedStatesMinorOutlyingIslands:
          printer_->setPageSize(QPrinter::Letter); break;
      default:
          printer_->setPageSize(QPrinter::A4); break;
    }
}

TextPrinter::~TextPrinter()
{
    delete printer_;
}

QPrinter::PageSize TextPrinter::pageSize() const
{
    return printer_->pageSize(); 
}

void TextPrinter::setPageSize(QPrinter::PageSize size)
{
    printer_->setPageSize(size);
}

QPrinter::Orientation TextPrinter::orientation() const
{
    return printer_->orientation();
}

void TextPrinter::setOrientation(QPrinter::Orientation orientation)
{
    // Valeur par defaut = QPrinter::Portrait
    printer_->setOrientation(orientation);
}

double TextPrinter::leftMargin() const
{
    return leftmargin_;
}

void TextPrinter::setLeftMargin(double margin)
{
    // Valeur par defaut = 15mm
    if ((margin > 0) && (margin < printer_->paperRect().width() / 2))
        leftmargin_ = margin;
    else
        leftmargin_ = 0;
}


double TextPrinter::rightMargin() const
{
    return rightmargin_;
}

void TextPrinter::setRightMargin(double margin)
{
    // Valeur par defaut = 10mm
    if ((margin > 0) && (margin < printer_->paperRect().width() / 2))
        rightmargin_ = margin;
    else
        rightmargin_ = 0;
}

double TextPrinter::topMargin() const
{
    return topmargin_;
}

void TextPrinter::setTopMargin(double margin)
{
    // Valeur par defaut = 15mm
    if ((margin > 0) && (margin < printer_->paperRect().height() / 4))
        topmargin_ = margin;
    else
        topmargin_ = 0;
}

double TextPrinter::bottomMargin() const
{
    return bottommargin_;
}

void TextPrinter::setBottomMargin(double margin)
{
    // Valeur par defaut = 15mm
    if ((margin > 0) && (margin < printer_->paperRect().height() / 4))
        bottommargin_ = margin;
    else
        bottommargin_ = 0;
}

void TextPrinter::setMargins(double margin)
{
    // Valeur par defaut = 15mm
    if ((margin > 0) 
            && (margin < printer_->paperRect().height() / 2)
            && (margin < printer_->paperRect().width() / 2))
        leftmargin_ = rightmargin_ = topmargin_ = bottommargin_ = margin;
    else
        leftmargin_ = rightmargin_ = topmargin_ = bottommargin_ = 0;
}

double TextPrinter::spacing() const
{
    return spacing_;
}

void TextPrinter::setSpacing(double spacing)
{
    // Espace entre le contenu de la page et les blocs d'en-tête et de pied
    // Valeur par defaut = 5mm
    if ((spacing > 0) && (spacing <= printer_->paperRect().height() / 8))
        spacing_ = spacing;
    else
        spacing_ = 0;
}

double TextPrinter::headerSize() const
{
    return headersize_;
}

void TextPrinter::setHeaderSize(double size)
{
    // Valeur par defaut = 0mm (pas d'en-tête
    if ((size > 0) && (size <= printer_->paperRect().height() / 8))
        headersize_ = size;
    else
        headersize_ = 0;
}

double TextPrinter::headerRule() const
{
    return headerrule_;
}

void TextPrinter::setHeaderRule(double pointsize)
{
    // Valeur par defaut = 0.5 pt (1/144 inch)
    headerrule_ = qMax(0.0, pointsize);
}

const QString &TextPrinter::headerText() const
{
    return headertext_;
}

void TextPrinter::setHeaderText(const QString &text)
{
    headertext_ = text;
}

double TextPrinter::footerSize() const
{
    return footersize_;
}

void TextPrinter::setFooterSize(double size)
{
    // Valeur par defaut = 0 mm (pas de pied de page)
    if ((size > 0) && (size <= printer_->paperRect().height() / 8))
        footersize_ = size;
    else
        footersize_ = 0;
}

double TextPrinter::footerRule() const
{
    return footerrule_;
}

void TextPrinter::setFooterRule(double pointsize)
{
    // Valeur par defaut = 0.5 pt (1/144 inch)
    footerrule_ = qMax(0.0, pointsize);
}

const QString &TextPrinter::footerText() const
{
    return footertext_;
}

void TextPrinter::setFooterText(const QString &text)
{
    footertext_ = text;
}

const QString &TextPrinter::dateFormat() const
{
    return dateformat_;
}

void TextPrinter::setDateFormat(const QString &format)
{
    dateformat_ = format;
}

void TextPrinter::setDuplex(const QPrinter::DuplexMode duplex)
{
    duplex_ = duplex;
}

bool TextPrinter::print(const QTextDocument *document, QString ficpdf, const QString &caption, bool AvecChoixImprimante, bool QueLePdf)
{
    if (!document) return false;
    if (!QueLePdf)
    {
        if (QPrinterInfo::availablePrinterNames().size() == 0)
        {
            UpMessageBox::Watch(Q_NULLPTR, tr("Aucune imprimante n'est\nconfigurée sur cet ordinateur!"));
            return false;
        }

        // imprime le document
        // show print dialog
        if (AvecChoixImprimante || printer_->printerName() == "")
        {
            if (QPrinterInfo::availablePrinterNames().size() > 1)
            {
                QPrintDialog dialog(printer_, parent_);
                dialog.setWindowTitle(caption.isEmpty() ? tr("Impression") : caption);
                if (dialog.exec() == QDialog::Rejected)
                    return false;
                else printer_= dialog.printer();
            }
            else if (!QPrinterInfo::defaultPrinter().isNull())
                printer_ = new QPrinter(QPrinterInfo::defaultPrinter());
            else
                printer_ = new QPrinter(QPrinterInfo::availablePrinters().at(0));
        }
        printer_->setOutputFormat(QPrinter::NativeFormat);
        printer_->setOutputFileName(QString());
        // print it
        tempdoc_ = document->clone();
        launchprint(printer_);
    }
    // enregistre le pdf
    if (ficpdf != "")
    {
        QString printerName = printer_->printerName();              // nécessaire parce que l'impression du pdf réinitialise le nom de l'imprimante
        printer_->setOutputFormat(QPrinter::PdfFormat);
        printer_->setOutputFileName(ficpdf);
        tempdoc_ = document->clone();
        launchprint(printer_);
        printer_->setOutputFormat(QPrinter::NativeFormat);
        printer_->setPrinterName(printerName);                      // nécessaire parce que l'impression du pdf réinitialise le nom de l'imprimante
    }
    delete tempdoc_;
    tempdoc_ = Q_NULLPTR;
    return true;
}

void TextPrinter::exportPdf(const QTextDocument *document, const QString &caption, const QString &filename)
{
    if (!document) return;

    // file save dialog
    QString dialogcaption = caption.isEmpty() ? "Export PDF" : caption;
    QString exportname;
    if (QFile::exists(filename))
        exportname = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at((0)) + "/" + filename;
    else
        exportname = QFileDialog::getSaveFileName(parent_, dialogcaption, filename, "*.pdf");
    if (exportname.isEmpty()) return;
    if (QFileInfo(exportname).suffix().isEmpty())
        exportname.append(".pdf");

    // setup printer
    printer_->setOutputFormat(QPrinter::PdfFormat);
    printer_->setOutputFileName(exportname);

    // print it
    tempdoc_ = document->clone();
    launchprint(printer_);

    delete tempdoc_;
    tempdoc_ = Q_NULLPTR;
}

bool TextPrinter::preview(const QTextDocument *document, QString ficpdf, const QString &caption)
{
    if (!document) return false;

    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(printer_, parent_);
    dialog->setWindowTitle(caption.isEmpty() ? "Print Preview" : caption);
    dialog->setWindowModality(Qt::WindowModal);
    connect(dialog, &QPrintPreviewDialog::paintRequested, this, &TextPrinter::launchprint);

    // preview it
    tempdoc_ = document->clone();
    QString printerName;
    int b = dialog->exec();
    if (b>0)
    {
        printer_= dialog->printer();
        printerName = printer_->printerName();
        // enregistre le pdf
        if (ficpdf != "")
        {
            printer_->setOutputFormat(QPrinter::PdfFormat);
            printer_->setOutputFileName(ficpdf);
            tempdoc_ = document->clone();
            launchprint(printer_);
            printer_->setOutputFormat(QPrinter::NativeFormat);
            printer_->setPrinterName(printerName); // nécessaire parce que l'impression du pdf réinitialise le nom de l'imprimante
        }
    }
    delete tempdoc_;
    tempdoc_ = Q_NULLPTR;
    delete dialog;
    return (b>0);
}

///////////////////////////////////////////////////////////////////////////////
// private methods
///////////////////////////////////////////////////////////////////////////////
void TextPrinter::setPrinterName(QString printerName)
{
    if (printerName != "") printer_->setPrinterName(printerName);
}

QString TextPrinter::getPrinterName()
{
    return printer_->printerName();
}


// paperRect() ////////////////////////////////////////////////////////////////
// Return the size of the paper, adjusted for DPI

QRectF TextPrinter::paperRect(QPaintDevice *device)
{
    // calculate size of paper
    QRectF rect = printer_->paperRect();
    // adjust for DPI
    rect.setWidth(rect.width() *
                  device->logicalDpiX() / printer_->logicalDpiX());
    rect.setHeight(rect.height() *
                  device->logicalDpiY() / printer_->logicalDpiY());

    return rect;
}

// contentRect() //////////////////////////////////////////////////////////////
// calculate the rect for the content block

QRectF TextPrinter::contentRect(QPaintDevice *device)
{
    // calculate size of content (paper - margins)
    QRectF rect = paperRect(device);

    rect.adjust(Utils::mmToInches(leftmargin_) * device->logicalDpiX(),
                Utils::mmToInches(topmargin_) * device->logicalDpiY(),
                -Utils::mmToInches(rightmargin_) * device->logicalDpiX(),
                -Utils::mmToInches(bottommargin_) * device->logicalDpiY());

    // header
    if (headersize_ > 0) {
        rect.adjust(0, Utils::mmToInches(headersize_) * device->logicalDpiY(), 0, 0);
        rect.adjust(0, Utils::mmToInches(spacing_) * device->logicalDpiY(), 0, 0);
    }
    // footer
    if (footersize_ > 0) {
        rect.adjust(0, 0, 0, -Utils::mmToInches(footersize_) * device->logicalDpiY());
        rect.adjust(0, 0, 0, -Utils::mmToInches(spacing_) * device->logicalDpiY());
    }

    return rect;
}

// headerRect() //////////////////////////////////////////////////////////////
// calculate the rect for the header block

QRectF TextPrinter::headerRect(QPaintDevice *device)
{
    QRectF rect = paperRect(device);
    rect.adjust(Utils::mmToInches(leftmargin_) * device->logicalDpiX(),
                Utils::mmToInches(topmargin_) * device->logicalDpiY(),
                -Utils::mmToInches(rightmargin_) * device->logicalDpiX(), 0);

    headerrule_ = (headerrule_ / 144.0);

    rect.setBottom(rect.top() + 
                   Utils::mmToInches(headersize_) * device->logicalDpiY());

    return rect;
}

// footerRect() ///////////////////////////////////////////////////////////////
// calculate the rect for the footer block

QRectF TextPrinter::footerRect(QPaintDevice *device)
{
    QRectF rect = paperRect(device);
    rect.adjust(Utils::mmToInches(leftmargin_) * device->logicalDpiX(), 0,
                -Utils::mmToInches(rightmargin_) * device->logicalDpiX(),
                -Utils::mmToInches(bottommargin_) * device->logicalDpiY());

    rect.setTop(rect.bottom() -
                Utils::mmToInches(footersize_) * device->logicalDpiY());

    return rect;
}

// print() ////////////////////////////////////////////////////////////////////
// Common printing routine. Print tempdoc_ to given printer device.

void TextPrinter::launchprint(QPrinter *printer)
{
    if (!printer || !tempdoc_) return;
    if (duplex_ == QPrinter::DuplexLongSide)
        printer_->setDuplex(duplex_);

    tempdoc_->setUseDesignMetrics(true);
    tempdoc_->documentLayout()->setPaintDevice(printer);
    tempdoc_->setPageSize(contentRect(printer).size());
    // dump existing margin (if any)
    QTextFrameFormat fmt = tempdoc_->rootFrame()->frameFormat();
    fmt.setMargin(0);
    tempdoc_->rootFrame()->setFrameFormat(fmt);

    // to iterate through pages we have to worry about
    // copies, collation, page range, and print order

    // get num copies
    int doccopies;
    int pagecopies;
    if (printer->collateCopies()) {
        doccopies = 1;
        pagecopies = printer->numCopies();
    } else {
        doccopies = printer->numCopies();
        pagecopies = 1;
    }

    // get page range
    int firstpage = printer->fromPage();
    int lastpage = printer->toPage();
    if (firstpage == 0 && lastpage == 0) { // all pages
        firstpage = 1;
        lastpage = tempdoc_->pageCount();
    }

    // print order
    bool ascending = true;
    if (printer->pageOrder() == QPrinter::LastPageFirst) {
        int tmp = firstpage;
        firstpage = lastpage;
        lastpage = tmp;
        ascending = false;
    }

    // loop through and print pages
    QPainter Imprimante(printer);
    Imprimante.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);
    for (int dc=0; dc<doccopies; dc++) {
        int pagenum = firstpage;
        while (true)
        {
            for (int pc=0; pc<pagecopies; pc++)
            {
                if (printer->printerState() == QPrinter::Aborted || printer->printerState() == QPrinter::Error)
                    return;
                // print page
                paintPage(&Imprimante, tempdoc_, pagenum, lastpage);
                if (pc < pagecopies-1) printer->newPage();
            }
            if (pagenum == lastpage) break;
            if (ascending)
                pagenum++;
            else
                pagenum--;
            printer->newPage();
        }
        if (dc < doccopies-1) printer->newPage();
    }
}

// paintPage() ////////////////////////////////////////////////////////////////
// paint an individual page of the document to the painter

void TextPrinter::paintPage(QPainter *Imprimante, QTextDocument *document, int pagenum, int nbpages)
{
    QRectF rect;
    double onepoint = Imprimante->device()->logicalDpiY() / 72.0;

    // header
    if (headersize_ > 0) {
        rect = headerRect(Imprimante->device());
        if (headerrule_ > 0.0) {
            Imprimante->save();
            // allow space between rule and header
            Imprimante->translate(0, onepoint + (headerrule_ * onepoint / 2.0));        // déplace le pinceau en bas à gauche de l'en-tête
            Imprimante->setPen(QPen(Qt::black, headerrule_ * onepoint));                // choisit le format du pinceau
            Imprimante->drawLine(rect.bottomLeft(), rect.bottomRight());                // trace une ligne depuis le bas à gauche au bas à droite de l'en-tête
            Imprimante->restore();
        }

        // replace page variables
        QString header = headertext_;
        header.replace("&page;", QString::number(pagenum) + "/" + QString::number(nbpages));
        if (dateformat_.isEmpty())
            header.replace("&date;", QDate::currentDate().toString());
        else
            header.replace("&date;", QDate::currentDate().toString(dateformat_));

        Imprimante->save();
        Imprimante->translate(rect.left(), rect.top());
        QRectF clip(0, 0, rect.width(), rect.height());
        QTextDocument doc;
        doc.setUseDesignMetrics(true);
        doc.setHtml(header);
        doc.documentLayout()->setPaintDevice(Imprimante->device());
        doc.setPageSize(rect.size());

        // align text to bottom
        double newtop = clip.bottom() - doc.size().height();
        clip.setHeight(doc.size().height());
        Imprimante->translate(0, newtop-50);
        doc.drawContents(Imprimante, clip);
        Imprimante->restore();
    }

    // footer
    if (footersize_ > 0) {
        rect = footerRect(Imprimante->device());
        if (footerrule_ > 0.0) {
            Imprimante->save();
            // allow space between rule and footer
            Imprimante->translate(0, -onepoint + (-footerrule_ * onepoint / 2.0));
            Imprimante->setPen(QPen(Qt::black, footerrule_ * onepoint));
            Imprimante->drawLine(rect.topLeft(), rect.topRight());
            Imprimante->restore();
        }

        // replace page variables
        QString footer = footertext_;
        footer.replace("&page;", QString::number(pagenum) + "/" + QString::number(nbpages));
        if (dateformat_.isEmpty())
            footer.replace("&date;", QDate::currentDate().toString());
        else
            footer.replace("&date;", QDate::currentDate().toString(dateformat_));

        Imprimante->save();
        Imprimante->translate(rect.left(), rect.top());
        QRectF clip(0, 0, rect.width(), rect.height());
        QTextDocument doc;
        doc.setUseDesignMetrics(true);
        doc.setHtml(footer);
        doc.documentLayout()->setPaintDevice(Imprimante->device());
        doc.setPageSize(rect.size());
        doc.drawContents(Imprimante, clip);
        Imprimante->restore();
    }

    // content
    Imprimante->save();

    rect = contentRect(Imprimante->device());
    Imprimante->translate(rect.left(), rect.top() - (pagenum-1) * rect.height());
    QRectF clip(0, (pagenum-1) * rect.height(), rect.width(), rect.height());

    document->drawContents(Imprimante, clip);

    Imprimante->restore();
}
