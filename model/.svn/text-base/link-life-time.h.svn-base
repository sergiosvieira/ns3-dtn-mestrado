/*
 * LinkLifeTime.h
 *
 *  Created on: 10/01/2012
 *      Author: sergio
 */

#ifndef LINKLIFETIME_H_
#define LINKLIFETIME_H_

#include "ns3/vector2d.h"
#include "ns3/nstime.h"

namespace ns3 {

class LinkLifeTime {
public:
        /*
         inline static Time getExpirationTime(Vector2d pos_a, Vector2d vel_a,
         Vector2d pos_b, Vector2d vel_b, double transmission_range) {
         Time result;
         double a, b, c, d, r;

         a = vel_a.x - vel_b.x;
         b = pos_a.x - pos_b.x;
         c = vel_a.y - vel_b.y;
         d = pos_a.y - pos_b.y;
         r = transmission_range;

         double let = (-(a * b + c * d) + sqrt(pow(a, 2.0) + pow(c, 2.0) * pow(
         r, 2.0) - pow((a * d - b * c), 2.0))) / (pow(1, 2.0) + pow(c,
         2.0));

         result = Time::FromDouble(let, Time::S);

         return result;

         }
         */
        inline static Time getExpirationTime(Vector2d pos_a, Vector2d vel_a,
                        Vector2d pos_b, Vector2d vel_b, double transmission_range) {
                double inf = 50.0;
                Time result;
                Vector2d my_position = pos_a;
                Vector2d my_velocity = vel_a;
                Vector2d neighbour_position = pos_b;
                Vector2d neighbour_velocity = vel_b;
                double trange = transmission_range;

                double let, a, b, c, x_1, x_2, delta;
                //se os carros estão parados
                if ((my_velocity.x == 0.0) && (my_velocity.y == 0.0)
                                && (neighbour_velocity.x == 0.0) && (neighbour_velocity.y
                                == 0.0)) {
                        let = inf;
                } else { //pelo menos um dos carros está em movimento
                        if (my_velocity.x - neighbour_velocity.x == 0.0) {
                                neighbour_velocity.x = neighbour_velocity.x + 0.00001;
                        }
                        if (my_velocity.y - neighbour_velocity.y == 0.0) {
                                neighbour_velocity.y = neighbour_velocity.y + 0.00001;
                        }
                        if (my_position.x - neighbour_position.x == 0.0) {
                                neighbour_position.x = neighbour_position.x + 0.00001;
                        }
                        if (my_position.y - neighbour_position.y == 0.0) {
                                neighbour_position.y = neighbour_position.y + 0.00001;
                        }

                        a = pow((my_velocity.x - neighbour_velocity.x), 2.0) + pow(
                                        (my_velocity.y - neighbour_velocity.y), 2.0);
                        b = 2 * ((my_position.x - neighbour_position.x) * (my_velocity.x
                                        - neighbour_velocity.x) + (my_position.y
                                        - neighbour_position.y) * (my_velocity.y
                                        - neighbour_velocity.y));
                        c = pow((my_position.x - neighbour_position.x), 2.0) + pow(
                                        (my_position.y - neighbour_position.y), 2.0) - pow(trange,
                                        2.0);
                        if (a == 0.0) {
                                a = 0.000000001;
                        }
                        delta = pow(b, 2.0) - (4.0 * a * c);

                        if (delta > 0.0) {
                                x_1 = (-b + pow(delta, 0.5)) / (2.0 * a);
                                x_2 = (-b - pow(delta, 0.5)) / (2.0 * a);
                                if (x_1 > 0.0) {
                                        let = x_1;
                                } else if (x_2 > 0.0) {
                                        let = x_2;
                                }
                        } else if (delta == 0.0) {
                                let = (-b) / (2.0 * a);
                        } else {
                                let = inf;
                        }
                }

                if (let > inf) {
                        let = inf;
                }

                result = Time::FromDouble(Simulator::Now().GetSeconds() + let, Time::S);

                return result;
        }

};

#endif /* LINKLIFETIME_H_ */

}
