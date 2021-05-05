/**
 * @file tools.h
 *
 * @brief Ensemble de fonction permettant d'aider lors du debuggage.
 *
 * Definie plusieurs macro:
 *
 * #STOP_ON_ERROR
 * #TRACE
 * #ARRAY_SIZE
 * #PRINT
 * #DEBUG_FILE_PATH
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-louis
 * @copyright BSD 2-clauses
 *
 */

#ifndef DEBUG_TOOLS_
#define DEBUG_TOOLS_

#include <unistd.h>
#include <stdio.h>

#define DEBUG_FILE_PATH "./debug.txt"

/**
 * @def PRINT
 *
 * @brief Affiche un message.
 *
 * Cette macro peut être utilisee comme la fonction printf,
 * le premier argument est la chaîne de caractere et les arguments
 * suivants peuvent être specifies en fonction du format choisi.
 *
 * Le flux peut être redirige vers un fichier si la macro NDEBUG
 * est defini.
 *
 * @param fmt chaine de charactere  formatee, voir la documentation de #printf.
 * @param
 */
#ifndef NDEBUG
#define PRINT(fmt, ...)                                         \
    do                                                          \
    {                                                           \
        fprintf(stderr, fmt, ##__VA_ARGS__);                    \
        fflush(stderr);                                         \
    } while (0)
#else
#define PRINT(fmt, ...)                                         \
    do                                                          \
    {                                                           \
        FILE *stream = fopen(DEBUG_FILE_PATH, "a");                 \
        fprintf(stream, fmt, ##__VA_ARGS__);                    \
        fclose(stream);                                         \
    } while (0)
#endif

/**
 * @def STOP_ON_ERROR
 *
 * @brief Arrête l'execution en cas d'erreur et fournit des informations utiles.
 * sur l'erreur.
 *
 * Cette macro est utile pour evacuer la gestion des erreurs pour les experimentations.
 *
 * @param error_condition doit être vrai pour signifier qu'il y a une erreur, alors
 * l'execution s'arrête avec un message d'erreur, rien n'est fait si la condition est fausse.
 *
 * @warning Cette macro ne doit pas être utilisee dans du code de production
 * ni dans aucun code pouvant arriver a un etat de production car
 * c'est un moyen de ne pas gerer les erreurs. C'est pourquoi vous obtiendrez
 * une erreur de preprocesseur si vous definissez la macro NDEBUG.
 */
#ifndef NDEBUG
#define STOP_ON_ERROR(error_condition)                            \
    do                                                            \
    {                                                             \
        if (error_condition)                                      \
        {                                                         \
            PRINT("*** Error (%s) at %s:%d\nExiting\n",           \
                    #error_condition, __FILE__, __LINE__);        \
            perror("");                                           \
            _exit(1);                                             \
        }                                                         \
    } while (0)
#else
#define STOP_ON_ERROR(error_condition)                            \
    do                                                            \
    {                                                             \
        if (error_condition)                                      \
        {                                                         \
            PRINT("*** Error (%s) at %s:%d\nExiting\n",           \
                    #error_condition, __FILE__, __LINE__);        \
            perror("");                                           \
            _exit(1);                                             \
        }                                                         \
    } while (0)

//#define STOP_ON_ERROR(error_condition) #error "STOP_ON_ERROR must not be used in release builds"
#endif

/**
 * @def TRACE
 *
 * @brief Affiche un message de debug.
 *
 * Cette macro peut être utilisee comme la fonction printf,
 * le premier argument est la chaîne de caracteres, les arguments
 * suivants peuvent être specifies en fonction du format choisi.
 *
 * L'affichage peut être desactivee en definissant la macro NDEBUG.
 *
 * @param fmt chaine de caractere formatee, voir la documentation de #printf.
 */
#ifndef NDEBUG
#define TRACE(fmt, ...)                                         \
    PRINT("%s:%d:%s(): " fmt, __FILE__, __LINE__, \
                __func__, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)
#endif

/**
 * @def ARRAY_COUNT
 *
 * @brief Donne la taille d'un tableau sans tenir compte de la taille de chaque element.
 *
 * @param array Le tableau.
 */
#define ARRAY_COUNT(array) ((sizeof(array)) / sizeof(array[0]))

#endif /* DEBUG_TOOLS_ */
