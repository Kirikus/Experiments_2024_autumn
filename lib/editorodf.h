#ifndef __EDITORODF_H__
#define __EDITORODF_H__

#include <QList>

class Block {
public:
    virtual void update();
    virtual void getVisibleWidget();
    virtual void export();
}

class EditorODF {
private:
    QList<Block> blocks;
public:
    void removeBlock(Block block) {};
    void addPlotBlock() {};
    void addTextBlock() {};
    void addTableBlock() {};
    void export() {};
    void serManager() {};
    void moveBlockUp() {};
    void moveBlockDown() {};
}

#endif  // __EDITORODF_H__