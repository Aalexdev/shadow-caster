#ifndef __FILTER__hpp__
#define __FILTER__hpp__

    class Light;

    #include "main.hpp"
    #include <vector>

    #define DRAW_CELL false
    #define DRAW_EDGE false

    class Light{
        public:
            Light(MainVar* mainVar);
            ~Light();

            bool draw(void);
            

        /**
         * @brief texture and rendering vars
         * 
         */
        private:
            MainVar* mainVar;
        
        /**
         * @brief dynamique light function
         * 
         */
        public:
            /**
             * @brief set a point one the screen, will be detected by the dynamic light
             * 
             * @param x the x position
             * @param y the y position
             */
            void pushPoint(int x, int y);

            /**
             * @brief set a rectagle on the screen, will be detected by the dynamic light
             * 
             * @param x the x position
             * @param y the y position
             * @param w the width of the rect
             * @param h the height of the rect
             */
            void pushRect(int x, int y, int w, int h);

            /**
             * @brief set a filled rectagle on the screen, will be detected by the dynamic light
             * 
             * @param x the x position
             * @param y the y position
             * @param w the width of the rect
             * @param h the height of the rect
             */
            void pushRectF(int x, int y, int w, int h);

            /**
             * @brief draw a line from the fisrt coord to the second coord, will be detected by light
             * 
             * @param x1 x axis of the starting point
             * @param y1 y axis of the starting point
             * @param x2 x axis of the second point
             * @param y2 y axis of the second point
             * @param a the alpha of the line
             * @return return true if drawn, false on error
             */
            void pushLine(float x1, float y1, float x2, float y2);

            /**
             * @brief draw a circle, will be detected by the light
             * 
             * @param x 
             * @param y 
             * @param r 
             */
            void pushCircle(int x, int y, int r);

            /**
             * @brief print the screen in the terminal
             * 
             */
            void print(void);

            /**
             * @brief convert the map into a vector
             * 
             * @param sx starting x
             * @param sy starting y
             * @param w width
             * @param h height
             * @param pitch 
             */
            void convertTileMapToPolyMap(int sx, int sy, int w, int h, int pitch);

            /**
             * @brief calculate the visibility from the center O
             * 
             * @param ox center x axis
             * @param oy center y axis
             * @param radius radius of visibility
             */
            void calculateVisibilityPolygon(float ox, float oy, float radius);

            /**
             * @brief push a new light source on the screen
             * 
             * @param x the x axis
             * @param y teh y axis
             */
            void pushLightSource(int x, int y);

            /**
             * @brief clear all lights in the screen
             * 
             */
            void clearLight(void);

        /**
         * @brief dynamic light vars and function
         * 
         */
        private:

            struct sEdge{
                float sx, sy;
                float ex, ey;
            };

            /**
             * @brief cellule structure
             * 
             */
            struct sCell{
                int edge_id[4];
                bool edge_exist[4];
                bool exist = false;
            };

            enum Egde_ID{
                NORTH = 0,
                SOUTH = 1,
                EAST = 2,
                WEST = 3
            };

            sCell* world;

            vector<sEdge> vecEdges;

            void drawEdges(void);

            vector<tuple<float, float, float>> vecVisibilityPolygonPoints;

            vector<SDL_Point> lightSources;
    };


#endif /* __FILTER__hpp__ */