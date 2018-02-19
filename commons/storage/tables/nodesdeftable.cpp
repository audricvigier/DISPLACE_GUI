#include "nodesdeftable.h"

#include "Node.h"

struct NodesDefTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Text>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> statement;
};

NodesDefTable::NodesDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{    
}

NodesDefTable::~NodesDefTable() noexcept = default;

void NodesDefTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldNodeName,
                           fldLong, fldLat
                           ));
}

void NodesDefTable::insert(Node *node)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldNodeId,
                                                     fldNodeName,
                                                     fldLong, fldLat));
    }

    SQLiteTable::insert(p->statement, std::make_tuple((int)node->get_idx_node().toIndex(),
                        node->get_name(),
                        node->get_x(),
                        node->get_y())
            );
}