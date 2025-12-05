#include "figura.h"

#ifndef QUADRADO_H
#define QUADRADO_H

class Quadrado : public Figura
{
    private:
    public:
        Quadrado(int x=0, int y=0);
        virtual ~Quadrado();
        virtual void desenhar() const;
};

#endif