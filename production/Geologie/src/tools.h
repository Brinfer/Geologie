/**
 * @file tools.h
 *
 * @brief Ensemble de fonction permettant d'aider lors du debuggage.
 *
 * Definie plusieurs macro:
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef DEBUG_TOOLS_
#define DEBUG_TOOLS_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEBUG_FILE_PATH "./debug.log"

/**
 * @def LOG
 *
 * @brief Affiche un message.
 *
 * Cette macro peut etre utilisee comme la fonction printf,
 * le premier argument est la chaene de caractere et les arguments
 * suivants peuvent etre specifies en fonction du format choisi.
 *
 * Le flux peut etre redirige vers un fichier si la macro NDEBUG
 * est defini.
 *
 * @param fmt chaine de charactere  formatee, voir la documentation de #printf.
 * @param
 */
#ifndef NDEBUG
#define LOG(fmt, ...)                                           \
    do {                                                        \
        fprintf(stderr, fmt, ##__VA_ARGS__);                    \
        fflush(stderr);                                         \
    } while (0)
#else
#define LOG(fmt, ...)                                           \
    do {                                                        \
        FILE *stream = fopen(DEBUG_FILE_PATH, "a");             \
        fprintf(stream, fmt, ##__VA_ARGS__);                    \
        fclose(stream);                                         \
    } while (0)
#endif

/**
 * @def ERROR
 *
 * @brief Affiche un message message si la condition passe est vrai.
 *
 * Dans le cas ou errno est different de 0, alors le message liee a errno est lui
 * aussi affiche.
 *
 * Suite a l'affiche, errno est remis a 0.
 *
 * Le flux peut etre redirige vers un fichier si la macro NDEBUG
 * est defini.
 *
 * @param error_condition La condition pour qu'il y est une erreur est que le message soit affiche.
 * @param fmt chaine de charactere  formatee, voir la documentation de #printf.
 */
#ifndef NDEBUG
#define ERROR(error_condition, fmt)                                         \
    do {                                                                    \
        if (error_condition) {                                              \
            if (errno != 0) {                                               \
                perror(fmt);                                                \
                errno = 0;                                                  \
            } else {                                                        \
                fprintf(stderr, fmt);                                       \
            }                                                               \
            fprintf(stderr, "\n");                                          \
        }                                                                   \
    } while (0)
#else
#define ERROR(error_condition, fmt)                                         \
    do {                                                                    \
        if (error_condition) {                                              \
            FILE *stream = fopen(DEBUG_FILE_PATH, "a");                     \
            if (errno != 0) {                                               \
                fprintf(stream, "%s : %s\n", fmt, strerror_r(errno));       \
                errno = 0;                                                  \
            } else {                                                        \
                fprintf(stream,"%s\n", fmt);                                \
            }                                                               \
            fclose(stream);                                                 \
        }                                                                   \
    } while (0)
#endif

/**
 * @def STOP_ON_ERROR
 *
 * @brief Arrete l'execution en cas d'erreur et fournit des informations utiles.
 * sur l'erreur.
 *
 * Cette macro est utile pour evacuer la gestion des erreurs pour les experimentations.
 *
 * @param error_condition doit etre vrai pour signifier qu'il y a une erreur, alors
 * l'execution s'arrete avec un message d'erreur, rien n'est fait si la condition est fausse.
 *
 * @warning Cette macro ne doit pas etre utilisee dans du code de production
 * ni dans aucun code pouvant arriver a un etat de production car
 * c'est un moyen de ne pas gerer les erreurs. C'est pourquoi vous obtiendrez
 * une erreur de preprocesseur si vous definissez la macro NDEBUG.
 */
#ifndef NDEBUG
#define STOP_ON_ERROR(error_condition)                            \
    do {                                                          \
        if (error_condition) {                                    \
            fprintf(stderr, "*** Error (%s) at %s:%d\nExiting\n", \
                    #error_condition, __FILE__, __LINE__);        \
            fflush(stderr);                                       \
            perror("");                                           \
            _exit(1);                                             \
        }                                                         \
    } while (0)
#else
#define STOP_ON_ERROR(error_condition) #error "STOP_ON_ERROR must not be used in release builds"
#endif

/**
 * @def TRACE
 *
 * @brief Affiche un message de debug.
 *
 * Cette macro peut etre utilisee comme la fonction printf,
 * le premier argument est la chaene de caracteres, les arguments
 * suivants peuvent etre specifies en fonction du format choisi.
 *
 * L'affichage peut etre desactivee en definissant la macro NDEBUG.
 *
 * @param fmt chaine de caractere formatee, voir la documentation de #printf.
 */
#ifndef NDEBUG
#define TRACE(fmt, ...)                                            \
    do {                                                           \
        fprintf (stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__,   \
			__func__, ##__VA_ARGS__);                              \
    } while (0)
#else
#define TRACE(fmt, ...)
#endif

/**
 * @brief Donne la taille d'un tableau sans tenir compte de la taille de chaque element.
 *
 * @param array Le tableau.
 */
#define ARRAY_COUNT(array) ((sizeof(array)) / sizeof(array[0]))


#define ARRAY_CONCATENATE(arraySource, arrayDest, indexStart, indexStop)    \
    do {                                                                    \
        for(int i = 0; i < (indexStop - indexStart); i++) {                 \
            arrayDest[indexStart + i] = arraySource[i];                     \
        }                                                                   \
    } while(0)

#endif /* DEBUG_TOOLS_ */
