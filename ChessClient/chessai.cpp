#include "chessai.h"

// Структура хода бота с его весом
struct STEP {
    int i1 ;
    int j1 ;
    int i2 ;
    int j2 ;
    int weight ;
} ;

bool ChessAI::doStepAI(Game * game)
{
    // Строим все возможные ходы
    QList<STEP> fullsteps ;
    auto ea = game->buildEnemyAttacked() ;
    for (int i=0; i<SZ; i++)
        for (int j=0; j<SZ; j++)
            if (game->isPlayerFigure(i,j))
                foreach(auto p, game->getSteps(i,j)) {                    
                    // Если идем в шах, то пропуск
                    if (game->isStepToCheck(i,j,p.x(),p.y())) continue ;

                    int w = 0 ;
                    // Если на ходу уводим фигуру из удара - вес хода 1
                    if ((ea.contains(QPoint(i,j)))&&(!ea.contains(p))) w = 1 ;
                    // Если на ходу берем фигуру - вес хода 2
                    if (game->isEnemyFigure(p.x(),p.y())) w = 2 ;
                    // Иначе просто вес 0, случайный ход
                    fullsteps.append(STEP { i,j,p.x(),p.y(),w }) ;
                }

    if (fullsteps.count()==0) return false ;

    // Сортируем ходы по весам
    std::sort(fullsteps.begin(),fullsteps.end(),
              [](const STEP &v1, const STEP &v2)
                {
                    return v1.weight>v2.weight ;
                }) ;

    STEP sel ;
    // Если самый лучший ход с весом ноль, то берем случайный, иначе берем лучший
    if (fullsteps[0].weight>0)
        sel = fullsteps[0] ;
    else
        sel = fullsteps[rand()%fullsteps.length()] ;

    // И делаем ход как бы от игрока, но из бота
    game->doStep(sel.i1,sel.j1,sel.i2,sel.j2,true) ;

    return true ;
}
