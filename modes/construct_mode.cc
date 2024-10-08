// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "construct_mode.h"

#include "../misc/argsparser.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"
#include "../objects/text_type.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/object_constructor.h"

#include "popup/objectchooserpopup.h"
#include "popup/popup.h"



#include <QMouseEvent>
#include <algorithm>
#include <functional>
#include <iterator>

static void redefinePoint(ObjectTypeCalcer *mpt, const Coordinate &c, KigDocument &doc, const KigWidget &w)
{
    ObjectFactory::instance()->redefinePoint(mpt, c, doc, w);
    mpt->calc(doc);
}

BaseConstructMode::BaseConstructMode(KigPart &d)
    : BaseMode(d)
{
    mpt = ObjectFactory::instance()->fixedPointCalcer(Coordinate(0, 0));
    mpt->calc(d.document());
    mcursor = ObjectFactory::instance()->cursorPointCalcer(Coordinate(0, 0));
    mcursor->calc(d.document());
    mdoc.startObjectGroup();
}

BaseConstructMode::~BaseConstructMode()
{
    mdoc.finishObjectGroup();

    delete mcursor;
}

void BaseConstructMode::leftReleased(QMouseEvent *e, KigWidget *v)
{
    if ((pointLocation() - e->pos()).manhattanLength() > 4)
        return;

    ObjectHolder *o = nullptr;
    bool keyCtrlOrShift = (e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) != 0;
    std::vector<ObjectHolder *> moco = oco();
    if (!moco.empty()) {
        std::vector<ObjectHolder *> goodargs;
        if (!moco.empty()) {
            std::vector<ObjectHolder *>::const_iterator it;
            std::vector<ObjectCalcer *> testargs = getCalcers(mparents);
            for (std::vector<ObjectHolder *>::const_iterator i = moco.begin(); i != moco.end(); ++i) {
                it = std::find(mparents.begin(), mparents.end(), *i);
                bool newdup = (it == mparents.end()) || isAlreadySelectedOK(testargs, it - mparents.begin());
                if (newdup) {
                    testargs.push_back((*i)->calcer());
                    if (wantArgs(testargs, mdoc.document(), *v))
                        goodargs.push_back(*i);
                    testargs.pop_back();
                }
            }
            int id = ObjectChooserPopup::getObjectFromList(e->pos(), v, goodargs);
            if (id >= 0)
                o = goodargs[id];
        }
    }
    leftClickedObject(o, e->pos(), *v, keyCtrlOrShift);
    KigMode::leftReleased(e, v);
}

void BaseConstructMode::leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool)
{
    std::vector<ObjectHolder *>::iterator it = std::find(mparents.begin(), mparents.end(), o);
    std::vector<ObjectCalcer *> nargs = getCalcers(mparents);
    //
    // mp: duplicationchecked controls whether the arguments list is
    // free of duplications or if a duplication is safe (asking this to
    // the Constructor class through the "isAlreadySelectedOK" method).
    //
    bool duplicationchecked = (it == mparents.end()) || isAlreadySelectedOK(nargs, it - mparents.begin());
    if (o && duplicationchecked) {
        nargs.push_back(o->calcer());
        if (wantArgs(nargs, mdoc.document(), w)) {
            selectObject(o, w);
            return;
        }
    }

    nargs = getCalcers(mparents);
    nargs.push_back(mpt.get());
    if (wantArgs(nargs, mdoc.document(), w)) {
        // add mpt to the document..
        ObjectHolder *n = new ObjectHolder(mpt.get());
        mdoc.addObject(n);
        selectObject(n, w);
        // get a new mpt for our further use..
        mpt = ObjectFactory::instance()->sensiblePointCalcer(w.fromScreen(p), mdoc.document(), w);
        mpt->calc(mdoc.document());
        return;
    }

    nargs = getCalcers(mparents);
    nargs.push_back(mcursor);

    if (wantArgs(nargs, mdoc.document(), w)) {
        // DON'T add mpt to the document..
        // the objectholder has been constructed once and for all
        // when entering construction mode, and delete in the
        // destructor.
        ObjectHolder *n = new ObjectHolder(mcursor);
        selectObject(n, w);
        mcursor = ObjectFactory::instance()->cursorPointCalcer(w.fromScreen(p));
        //    mcursor = ObjectFactory::instance()->sensiblePointCalcer( w.fromScreen( p ), mdoc.document(), w );
        mcursor->calc(mdoc.document());
        delete n;
    }
}

void BaseConstructMode::midClicked(const QPoint &p, KigWidget &w)
{
    std::vector<ObjectCalcer *> args = getCalcers(mparents);
    args.push_back(mpt.get());
    if (wantArgs(args, mdoc.document(), w)) {
        ObjectHolder *n = new ObjectHolder(mpt.get());
        mdoc.addObject(n);

        selectObject(n, w);

        mpt = ObjectFactory::instance()->sensiblePointCalcer(w.fromScreen(p), mdoc.document(), w);
        mpt->calc(mdoc.document());
    }
}

void BaseConstructMode::rightClicked(const std::vector<ObjectHolder *> &, const QPoint &, KigWidget &)
{
    cancelConstruction();
}

void BaseConstructMode::mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftpressed)
{
    // mdoc.emitStatusBarText(selectStatement(getCalcers(mparents), w)); //TODO port from QString to KLazyLocalizedString

    w.updateCurPix();
    KigPainter pter(w.screenInfo(), &w.curPix, mdoc.document());

    Coordinate ncoord = w.fromScreen(p);
    if (shiftpressed)
        ncoord = mdoc.document().coordinateSystem().snapToGrid(ncoord, w);

    redefinePoint(mpt.get(), ncoord, mdoc.document(), w);
    mcursor->move(ncoord, mdoc.document());
    mcursor->calc(mdoc.document());

    std::vector<ObjectCalcer *> args = getCalcers(mparents);
    bool duplicationchecked = false;
    std::vector<ObjectHolder *> goodargs;
    if (!os.empty()) {
        std::vector<ObjectHolder *>::const_iterator it;
        std::vector<ObjectCalcer *> testargs = getCalcers(mparents);
        for (std::vector<ObjectHolder *>::const_iterator i = os.begin(); i != os.end(); ++i) {
            it = std::find(mparents.begin(), mparents.end(), *i);
            bool newdup = (it == mparents.end()) || isAlreadySelectedOK(args, it - mparents.begin());
            if (newdup) {
                testargs.push_back((*i)->calcer());
                if (wantArgs(testargs, mdoc.document(), w))
                    goodargs.push_back(*i);
                testargs.pop_back();
            }
            duplicationchecked |= newdup;
        }
    }
    bool calcnow = (goodargs.size() == 1) || ((goodargs.size() > 0) && (goodargs.front()->imp()->inherits(PointImp::stype())));
    if (calcnow) {
        args.push_back(goodargs.front()->calcer());
    }

    if (!os.empty() && duplicationchecked && calcnow) {
        handlePrelim(args, p, pter, w);

        w.setCursor(Qt::PointingHandCursor);
    } else {
        std::vector<ObjectCalcer *> args = getCalcers(mparents);
        args.push_back(mpt.get());
        std::vector<ObjectCalcer *> argscursor = getCalcers(mparents);
        argscursor.push_back(mcursor);
        bool text = true;
        if (wantArgs(args, mdoc.document(), w)) {
            ObjectDrawer d;
            d.draw(*mpt->imp(), pter, true);

            handlePrelim(args, p, pter, w);

            w.setCursor(Qt::PointingHandCursor);
        } else if (wantArgs(argscursor, mdoc.document(), w)) {
            ObjectDrawer d;
            //      d.draw( *mcursor->imp(), pter, true );

            handlePrelim(argscursor, p, pter, w);

            w.setCursor(Qt::CrossCursor);
        } else {
            w.setCursor(Qt::ArrowCursor);
            text = false;
        }
        if (!text && (goodargs.size() > 1)) {
            QString strwhich = i18n("Which object?");
            mdoc.emitStatusBarText(strwhich);

            QPoint textloc = p;
            textloc.setX(textloc.x() + 15);
            pter.drawTextStd(textloc, strwhich);

            w.setCursor(Qt::PointingHandCursor);
        }
    }
    w.updateWidget(pter.overlay());
}

void BaseConstructMode::selectObject(ObjectHolder *o, KigWidget &w)
{
    mparents.push_back(o);
    std::vector<ObjectCalcer *> args = getCalcers(mparents);

    if (wantArgs(args, mdoc.document(), w) == ArgsParser::Complete) {
        handleArgs(args, w);
    };

    w.redrawScreen(mparents);
}

PointConstructMode::PointConstructMode(KigPart &d)
    : BaseMode(d)
{
    // we add the data objects to the document cause
    // ObjectFactory::redefinePoint does that too, and this way, we can
    // depend on them already being known by the doc when we add the
    // mpt..
    mpt = ObjectFactory::instance()->fixedPointCalcer(Coordinate());
    mpt->calc(d.document());

    mdoc.emitStatusBarText(i18n("Click the location where you want to place the new point, or the curve that you want to attach it to..."));
}

PointConstructMode::~PointConstructMode()
{
}

void PointConstructMode::leftClickedObject(ObjectHolder *, const QPoint &, KigWidget &w, bool)
{
    mdoc.addObject(new ObjectHolder(mpt.get()));
    w.redrawScreen(std::vector<ObjectHolder *>());

    mdoc.emitStatusBarText(QString());
    mdoc.doneMode(this);
}

void PointConstructMode::midClicked(const QPoint &p, KigWidget &w)
{
    leftClickedObject(nullptr, p, w, true);
}

void PointConstructMode::rightClicked(const std::vector<ObjectHolder *> &, const QPoint &, KigWidget &)
{
    cancelConstruction();
}

void PointConstructMode::mouseMoved(const std::vector<ObjectHolder *> &, const QPoint &p, KigWidget &w, bool shiftpressed)
{
    w.updateCurPix();
    KigPainter pter(w.screenInfo(), &w.curPix, mdoc.document());

    Coordinate ncoord = w.fromScreen(p);
    if (shiftpressed)
        ncoord = mdoc.document().coordinateSystem().snapToGrid(ncoord, w);

    redefinePoint(mpt.get(), ncoord, mdoc.document(), w);

    ObjectDrawer d;
    d.draw(*mpt->imp(), pter, true);
    w.setCursor(Qt::BlankCursor);

    w.updateWidget(pter.overlay());
}

void BaseConstructMode::enableActions()
{
    BaseMode::enableActions();

    mdoc.aCancelConstruction->setEnabled(true);
}

void BaseConstructMode::cancelConstruction()
{
    mdoc.cancelObjectGroup();
    finish();
}

void PointConstructMode::enableActions()
{
    BaseMode::enableActions();

    mdoc.aCancelConstruction->setEnabled(true);
}

void PointConstructMode::cancelConstruction()
{
    mdoc.doneMode(this);
}

void BaseConstructMode::selectObjects(const std::vector<ObjectHolder *> &os, KigWidget &w)
{
    for (std::vector<ObjectHolder *>::const_iterator i = os.begin(); i != os.end(); ++i) {
        std::vector<ObjectCalcer *> args = getCalcers(mparents);
        assert(wantArgs(args, mdoc.document(), w) != ArgsParser::Complete);
        selectObject(*i, w);
    };
}

void ConstructMode::handlePrelim(const std::vector<ObjectCalcer *> &args, const QPoint &p, KigPainter &pter, KigWidget &w)
{
    // set the text next to the arrow cursor like in modes/normal.cc
    QPoint textloc = p;
    textloc.setX(textloc.x() + 15);

    mctor->handlePrelim(pter, args, mdoc.document(), w);

    KLazyLocalizedString o = mctor->useText(*args.back(), args, mdoc.document(), w);
    // pter.drawTextStd(textloc, o); //TODO port from QString to KLazyLocalizedString
}

int ConstructMode::isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const int &pos)
{
    return mctor->isAlreadySelectedOK(os, pos);
}

int ConstructMode::wantArgs(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w)
{
    return mctor->wantArgs(os, d, w);
}

void BaseConstructMode::finish()
{
    mdoc.doneMode(this);
}

ConstructMode::ConstructMode(KigPart &d, const ObjectConstructor *ctor)
    : BaseConstructMode(d)
    , mctor(ctor)
{
}

ConstructMode::~ConstructMode()
{
}

// does a test result have a frame by default ?
static const bool test_has_frame_dflt = true;

void TestConstructMode::handlePrelim(const std::vector<ObjectCalcer *> &os, const QPoint &p, KigPainter &pter, KigWidget &w)
{
    Args args;
    std::transform(os.begin(), os.end(), std::back_inserter(args), std::mem_fn(&ObjectCalcer::imp));

    // usetext
    KLazyLocalizedString usetext = mtype->argsParser().usetext(args.back(), args);
    QPoint textloc = p;
    textloc.setX(textloc.x() + 15);
    // pter.drawTextStd(textloc, usetext); //TODO port from QString to KLazyLocalizedString

    // test result
    ObjectImp *data = mtype->calc(args, mdoc.document());
    if (!data->valid())
        return;
    assert(data->inherits(TestResultImp::stype()));
    QString outputtext = static_cast<TestResultImp *>(data)->data();
    TextImp ti(outputtext, w.fromScreen(p + QPoint(-40, 30)), test_has_frame_dflt);
    ti.draw(pter);

    delete data;
}

TestConstructMode::TestConstructMode(KigPart &d, const ArgsParserObjectType *type)
    : BaseConstructMode(d)
    , mtype(type)
{
}

TestConstructMode::~TestConstructMode()
{
}

void ConstructMode::handleArgs(const std::vector<ObjectCalcer *> &args, KigWidget &w)
{
    mctor->handleArgs(args, mdoc, w);
    finish();
}

int TestConstructMode::isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const int &)
{
    return false;
}

int TestConstructMode::wantArgs(const std::vector<ObjectCalcer *> &os, KigDocument &, KigWidget &)
{
    return mtype->argsParser().check(os);
}

void TestConstructMode::handleArgs(const std::vector<ObjectCalcer *> &args, KigWidget &)
{
    mresult = new ObjectTypeCalcer(mtype, args);
    mresult->calc(mdoc.document());
    mdoc.emitStatusBarText(i18n("Now select the location for the result label."));
}

void TestConstructMode::leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool ctrlOrShiftDown)
{
    if (mresult) {
        QPoint qloc = p + QPoint(-40, 0);
        Coordinate loc = w.fromScreen(qloc);

        std::vector<ObjectCalcer *> parents;
        parents.push_back(new ObjectConstCalcer(new IntImp(test_has_frame_dflt)));
        parents.push_back(new ObjectConstCalcer(new PointImp(loc)));
        parents.push_back(new ObjectConstCalcer(new StringImp(QStringLiteral("%1"))));
        assert(mresult->imp()->inherits(TestResultImp::stype()));
        //    parents.push_back(
        //      new ObjectPropertyCalcer(
        //        mresult.get(), "test-result" ) );
        //    parents.back()->calc( mdoc.document() );

        /* (mp)
         * now we can refer directly to the TestResultImp, since it is also a StringImp
         * this creates a backward compatibility issue with kig save files: new versions of
         * kig can still read old saved files, but files created with the new kig version
         * cannot be read by old versions of kig.  This is necessary in order to allow a test
         * result to also carry its intrinsic boolean value and thus be used in further
         * constructions, e.g. as an argument to a python script.
         */

        parents.push_back(mresult.get());

        ObjectCalcer *ret = new ObjectTypeCalcer(TextType::instance(), parents);
        ret->calc(mdoc.document());
        mdoc.addObject(new ObjectHolder(ret));

        w.unsetCursor();
        mdoc.emitStatusBarText(QString());

        finish();
    } else
        BaseConstructMode::leftClickedObject(o, p, w, ctrlOrShiftDown);
}

void TestConstructMode::midClicked(const QPoint &p, KigWidget &w)
{
    if (mresult) {
        // nothing to be done here, really
    } else
        BaseConstructMode::midClicked(p, w);
}

void TestConstructMode::rightClicked(const std::vector<ObjectHolder *> &, const QPoint &, KigWidget &)
{
    cancelConstruction();
}

void TestConstructMode::mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftPressed)
{
    if (mresult) {
        w.setCursor(Qt::BlankCursor);

        w.updateCurPix();
        KigPainter pter(w.screenInfo(), &w.curPix, mdoc.document());

        QPoint qloc = p + QPoint(-40, 0);
        Coordinate loc = w.fromScreen(qloc);
        assert(dynamic_cast<const TestResultImp *>(mresult->imp()));
        TextImp ti(static_cast<const TestResultImp *>(mresult->imp())->data(), loc, test_has_frame_dflt);
        ObjectDrawer d;
        d.draw(ti, pter, false);

        w.updateWidget(pter.overlay());
    } else
        BaseConstructMode::mouseMoved(os, p, w, shiftPressed);
}

KLazyLocalizedString ConstructMode::selectStatement(const std::vector<ObjectCalcer *> &args, const KigWidget &w)
{
    return mctor->selectStatement(args, mdoc.document(), w);
}

KLazyLocalizedString TestConstructMode::selectStatement(const std::vector<ObjectCalcer *> &sel, const KigWidget &)
{
    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), mem_fn(&ObjectCalcer::imp));

    KLazyLocalizedString ret = mtype->argsParser().selectStatement(args);
    if (ret.isEmpty())
        return KLazyLocalizedString();
    return ret;
}

void PointConstructMode::redrawScreen(KigWidget *w)
{
    w->redrawScreen(std::vector<ObjectHolder *>());
}

void BaseConstructMode::redrawScreen(KigWidget *w)
{
    w->redrawScreen(std::vector<ObjectHolder *>());
}
