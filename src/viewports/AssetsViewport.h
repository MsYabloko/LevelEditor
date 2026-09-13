
#ifndef LEVELEDITOR_ASSETSVIEWPORT_H
#define LEVELEDITOR_ASSETSVIEWPORT_H

#include "../ViewPort.h"

class AssetsViewport : public ViewPortBehaviour
{
private:
    int current_tab = 0;
public:
    void Render(float x, float y, float width, float height, int id) override;

};


#endif //LEVELEDITOR_ASSETSVIEWPORT_H
