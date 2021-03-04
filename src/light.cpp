#include "light.hpp"

#include <cmath>
#include <algorithm>
#include <array>

#include "SDL2/SDL2_gfxPrimitives.h"


Light::Light(MainVar* mainVar) : mainVar(mainVar){
    world = new sCell[WINDOW_AIR];

}

Light::~Light(){
    delete[] world;
}

bool Light::draw(void){

    SDL_Rect temp = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    Uint8 r, g, b, a;

    for (SDL_Point &p : lightSources){
        calculateVisibilityPolygon(p.x, p.y, 150);

        if (vecVisibilityPolygonPoints.size() > 1){

            Sint16 x[vecVisibilityPolygonPoints.size()];
            Sint16 y[vecVisibilityPolygonPoints.size()];

            for (int i=0; i<vecVisibilityPolygonPoints.size(); i++){
                x[i] = get<1>(vecVisibilityPolygonPoints[i]);
                y[i] = get<2>(vecVisibilityPolygonPoints[i]);

            }

            filledPolygonRGBA(mainVar->getRenderer(), x, y, vecVisibilityPolygonPoints.size(), 255, 255, 255, 255);
        }
    }

    
   if (DRAW_EDGE) drawEdges();
    return true;
}

void Light::pushPoint(int x, int y){

    int i = y * WINDOW_WIDTH + x;
    world[i].exist = true;
}

void Light::pushRect(int x, int y, int w, int h){

    for (int xp=x; xp<w+x; xp++){
        pushPoint(xp, y);
    }

    for (int yp=y; yp<h+y; yp++){
        pushPoint(x, yp);
    }

    for (int xp=x; xp<w+x; xp++){
        pushPoint(xp, y+h);
    }

    for (int yp=y; yp<h+y; yp++){
        pushPoint(x+w, yp);
    }
}

void Light::print(void){
    for (int i=0; i<WINDOW_AIR; i++){
        sCell* cell = &world[i];
        cout << cell->exist;
    }
}

void Light::pushRectF(int x, int y, int w, int h){
    for (int px=x; px<x+w; px++){
        for (int py=y; py<y+h; py++){
            pushPoint(px, py);
        }
    }
}

void Light::pushLine(float x1, float y1, float x2, float y2){

    float m;

    if (x2 != x1){
        m = (y2 - y1) / (x2 - x1);
    }

    if (x2 != x1 && SDL_abs(m) <= 1.0f){
    
        if (x1 > x2){
            swap(x1, x2);
            swap(y1, y2);
        }

        const float m = (y2 - y1) / (x2 - x1);
        const float b = y1 - m * x1;

        for (int x = (int)x1; x< (int)x2; x++){
            const float y = m * (float)x + b;
            pushPoint(x, (int)y);
        }
    } else {
        if (y1 > y2){
            swap(x1, x2);
            swap(y1, y2);
        }

        const float w = (x2 - x1) / (y2 - y1);
        const float p = x1 - w * y1;

        for (int y = (int)y1; y< (int)y2; y++){
            const float x = w * (float)y + p;
            pushPoint(x, (int)y);
        }
    }
}

void Light::convertTileMapToPolyMap(int sx, int sy, int w, int h, int pitch){
    vecEdges.clear();

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            for (int j = 0; j < 4; j++)
            {
                world[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
                world[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
            }

    for (int x = 1; x < w - 1; x++){
        for (int y = 1; y < h - 1; y++)
        {
            int i = (y + sy) * pitch + (x + sx);			// This
            int n = (y + sy - 1) * pitch + (x + sx);		// Northern Neighbour
            int s = (y + sy + 1) * pitch + (x + sx);		// Southern Neighbour
            int w = (y + sy) * pitch + (x + sx - 1);	// Western Neighbour
            int e = (y + sy) * pitch + (x + sx + 1);	// Eastern Neighbour

            if (world[i].exist)
            {
                if (!world[w].exist)
                {
                    if (world[n].edge_exist[WEST])
                    {
                        vecEdges[world[n].edge_id[WEST]].ey += 1;
                        world[i].edge_id[WEST] = world[n].edge_id[WEST];
                        world[i].edge_exist[WEST] = true;
                    }
                    else
                    {
                        sEdge edge;
                        edge.sx = (sx + x) * 1; edge.sy = (sy + y) * 1;
                        edge.ex = edge.sx; edge.ey = edge.sy + 1;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        world[i].edge_id[WEST] = edge_id;
                        world[i].edge_exist[WEST] = true;
                    }
                }

                if (!world[e].exist)
                {
                    if (world[n].edge_exist[EAST])
                    {
                        vecEdges[world[n].edge_id[EAST]].ey += 1;
                        world[i].edge_id[EAST] = world[n].edge_id[EAST];
                        world[i].edge_exist[EAST] = true;
                    }
                    else
                    {
                        sEdge edge;
                        edge.sx = (sx + x + 1) * 1; edge.sy = (sy + y) * 1;
                        edge.ex = edge.sx; edge.ey = edge.sy + 1;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        world[i].edge_id[EAST] = edge_id;
                        world[i].edge_exist[EAST] = true;
                    }
                }

                if (!world[n].exist){
                    if (world[w].edge_exist[NORTH]){

                        vecEdges[world[w].edge_id[NORTH]].ex += 1;
                        world[i].edge_id[NORTH] = world[w].edge_id[NORTH];
                        world[i].edge_exist[NORTH] = true;

                    } else {
                        sEdge edge;
                        edge.sx = (sx + x) * 1; edge.sy = (sy + y) * 1;
                        edge.ex = edge.sx + 1; edge.ey = edge.sy;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        world[i].edge_id[NORTH] = edge_id;
                        world[i].edge_exist[NORTH] = true;
                    }
                }

                if (!world[s].exist){
                    if (world[w].edge_exist[SOUTH]){
                        vecEdges[world[w].edge_id[SOUTH]].ex += 1;
                        world[i].edge_id[SOUTH] = world[w].edge_id[SOUTH];
                        world[i].edge_exist[SOUTH] = true;

                    } else {
                        sEdge edge;
                        edge.sx = (sx + x) * 1; edge.sy = (sy + y + 1) * 1;
                        edge.ex = edge.sx + 1; edge.ey = edge.sy;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        world[i].edge_id[SOUTH] = edge_id;
                        world[i].edge_exist[SOUTH] = true;
                    }
                }
            }
        }
    }
}


void Light::drawEdges(void){
    int i=0;
    for (sEdge &e : vecEdges){
        lineRGBA(mainVar->getRenderer(), e.sx, e.sy, e.ex, e.ey, 255, 255, 255, 255);
        circleRGBA(mainVar->getRenderer(), e.sx, e.sy, 3, 255, 0, 0, 255);
        circleRGBA(mainVar->getRenderer(), e.ex, e.ey, 3, 255, 0, 0, 255);
    }
}

void Light::calculateVisibilityPolygon(float ox, float oy, float radius){
    vecVisibilityPolygonPoints.clear();

    for (auto &e1 : vecEdges)
    {
        for (int i = 0; i < 2; i++)
        {
            float rdx, rdy;
            rdx = (i == 0 ? e1.sx : e1.ex) - ox;
            rdy = (i == 0 ? e1.sy : e1.ey) - oy;

            float base_ang = atan2f(rdy, rdx);

            float ang = 0;
            for (int j = 0; j < 3; j++)
            {
                if (j == 0)	ang = base_ang - 0.0001f;
                if (j == 1)	ang = base_ang;
                if (j == 2)	ang = base_ang + 0.0001f;

                rdx = radius * cosf(ang);
                rdy = radius * sinf(ang);

                float min_t1 = INFINITY;
                float min_px = 0, min_py = 0, min_ang = 0;
                bool bValid = false;

                for (auto &e2 : vecEdges)
                {
                    float sdx = e2.ex - e2.sx;
                    float sdy = e2.ey - e2.sy;

                    if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
                    {
                        float t2 = (rdx * (e2.sy - oy) + (rdy * (ox - e2.sx))) / (sdx * rdy - sdy * rdx);
                        float t1 = (e2.sx + sdx * t2 - ox) / rdx;

                        if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
                        {
                            if (t1 < min_t1)
                            {
                                min_t1 = t1;
                                min_px = ox + rdx * t1;
                                min_py = oy + rdy * t1;
                                min_ang = atan2f(min_py - oy, min_px - ox);
                                bValid = true;
                            }
                        }
                    }
                }

                if(bValid)
                    vecVisibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
            }
        }
    }

    sort(
        vecVisibilityPolygonPoints.begin(),
        vecVisibilityPolygonPoints.end(),
        [&](const tuple<float, float, float> &t1, const tuple<float, float, float> &t2)
        {
            return get<0>(t1) < get<0>(t2);
        });

}


void Light::pushLightSource(int x, int y){
    SDL_Point p = {x, y};
    lightSources.push_back(p);
}

void Light::clearLight(void){
    lightSources.clear();
}

void Light::pushCircle(int x, int y, int r){
    int tick = SDL_GetTicks();
    for (int tx = x-r; tx < x+r; tx++){
        for (int ty = y-r; ty < y+r; ty++){
            int circleV = pow(tx - x, 2) + pow(ty - y, 2) - pow(r, 2);
            if (circleV <= 1){
                pushPoint(tx, ty);
            }
        }
    }
}