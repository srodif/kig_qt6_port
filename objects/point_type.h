// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"
#include "circle_imp.h"
#include "common.h"

class FixedPointType : public ArgsParserObjectType
{
    FixedPointType();
    ~FixedPointType();

    static const ArgsParser::spec argsspec[1];

public:
    static const FixedPointType *instance();

    bool inherits(int type) const override;

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    bool canMove(const ObjectTypeCalcer &ourobj) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &ourobj) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &ourobj, const Coordinate &to, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;

    QList<KLazyLocalizedString> specialActions() const override;
    void executeAction(int i, ObjectHolder &o, ObjectTypeCalcer &t, KigPart &d, KigWidget &w, NormalMode &m) const override;
};

class RelativePointType : public ArgsParserObjectType
{
    RelativePointType();
    ~RelativePointType();

    static const ArgsParser::spec argsspec[1];

public:
    static const RelativePointType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    bool canMove(const ObjectTypeCalcer &ourobj) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &ourobj) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &ourobj, const Coordinate &to, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;

    //  QList<KLazyLocalizedString> specialActions() const;
    //  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
    //                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

class ConstrainedRelativePointType : public ArgsParserObjectType
{
    ConstrainedRelativePointType();
    ~ConstrainedRelativePointType();

    static const ArgsParser::spec argsspec[1];

public:
    static const ConstrainedRelativePointType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    bool canMove(const ObjectTypeCalcer &ourobj) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &ourobj) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &ourobj, const Coordinate &to, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class CursorPointType : public ObjectType
{
    CursorPointType();
    ~CursorPointType();

public:
    static const CursorPointType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;

    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;
    bool canMove(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &ourobj, const Coordinate &to, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConstrainedPointType : public ArgsParserObjectType
{
    ConstrainedPointType();
    ~ConstrainedPointType();

public:
    static const ConstrainedPointType *instance();

    bool inherits(int type) const override;

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;

    bool canMove(const ObjectTypeCalcer &ourobj) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &ourobj) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &ourobj, const Coordinate &to, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;

    QList<KLazyLocalizedString> specialActions() const override;
    void executeAction(int i, ObjectHolder &, ObjectTypeCalcer &o, KigPart &d, KigWidget &w, NormalMode &m) const override;
};

class MidPointType : public ObjectABType
{
    MidPointType();
    ~MidPointType();

public:
    static const MidPointType *instance();
    // calcx was an overloaded calc, which produced a compilation warning
    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class GoldenPointType : public ObjectABType
{
    GoldenPointType();
    ~GoldenPointType();

public:
    static const GoldenPointType *instance();
    // calcx was an overloaded calc, which produced a compilation warning
    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class MeasureTransportType : public ObjectType
{
    MeasureTransportType();
    ~MeasureTransportType();

public:
    static const MeasureTransportType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;
};

class MeasureTransportTypeOld : public ArgsParserObjectType
{
    MeasureTransportTypeOld();
    ~MeasureTransportTypeOld();

public:
    static const MeasureTransportTypeOld *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class PointByCoordsType : public ArgsParserObjectType
{
    PointByCoordsType();
    ~PointByCoordsType();

public:
    static const PointByCoordsType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ProjectedPointType : public ArgsParserObjectType
{
    ProjectedPointType();
    ~ProjectedPointType();

public:
    static const ProjectedPointType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
