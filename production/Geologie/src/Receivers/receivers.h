/**
 * @file receivers.h
 *
 * @brief 
 * @version 1.0.1
 * @date 5 mai 2021
 * @author LECENNE Gabriel
 * @copyright BSD 2-clauses
 *
 */

#ifndef RECEIVERS_H_
#define RECEIVERS_H_

/* \struct Receivers_t
 * \brief  Structure de receivers définie dans receivers.c
 */
typedef struct {
} Receivers_t;

typedef struct {
    char name;
    int8_t rssi;
} BeaconsData;

extern void Receivers_getAllBeaconsData();

extern Receivers_t * Receivers_new();
extern void Receivers_free(Receivers_t * this);
extern void Receivers_start(Receivers_t * this);
extern void Receivers_stop(Receivers_t * this);


#endif /* RECEIVERS_H_ */
