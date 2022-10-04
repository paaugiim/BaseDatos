//Programa en C para crear una base de datos
//Incluir esta libreriﾭa para poder hacer las llamadas en shiva2.upc.es
//#include <my_global.h> 
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv)
{
	//Conector para acceder al servidor de bases de datos 
	MYSQL *conn;
	int err;
	// Estructura especial para almacenar resultados de consultas
	MYSQL_RES *id_jugadores;
	MYSQL_RES *ganadas_jugadori;
	MYSQL_ROW row_id_j;
	MYSQL_ROW row_ganadas;

	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//inicializar la conexion, indicando nuestras claves de acceso
	// al servidor de bases de datos (user,pass) y el nombre de la bbdd
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "bd", 0, NULL, 0);
	if (conn==NULL)
	{
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	// Pedimos el id de todos los jugadores con una consulta
	char consulta1[100];
	strcpy(consulta1, "SELECT jugadores.id FROM jugadores");
	
	err = mysql_query(conn, consulta1);
	if (err!=0){
		printf("Error al consultar datos de la basa %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	
	// Recojemos resultado en froma de estructura
	id_jugadores = mysql_store_result (conn);
	
	// Coje la primera fila de la estructura anterior
	row_id_j = mysql_fetch_row (id_jugadores);
	
	char resultado[500];	// Declaramos este char para almacenar el resultado que obtengamos y despues mostrarlo por consola
	
	if (row_id_j == NULL)
		printf ("No se han obtenido datos en la consulta\n");	// Seguridad por si la consulta no se hace bien
	else{
		int record = 0;
		while (row_id_j !=NULL) 	// Recorremos la base de datos a traves d la tabla de id de jugador
		{
			// Pedimos a la base de datos el numero de partidas ganadas por el jugador con el id que estamos analizando
			char consulta2[100];
			strcpy(consulta2, "SELECT COUNT(partidas.id) FROM (partidas,registro,jugadores) WHERE jugadores.id =");
			strcat(consulta2, row_id_j[0]); // Aqui es on afegim a la consulta el id del jugador que estem analitzant 
			strcat(consulta2, " AND jugadores.id = registro.id_j AND registro.id_p = partidas.id");
			strcat(consulta2, " AND partidas.resultado = 'Victoria'");
			
			err = mysql_query(conn, consulta2);
			if (err!=0){
				printf("Error al consultar datos de la basa %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit(1);
			}
			// Recojemos los resultados e iremos haciendo lo mismo que antes, tendremos una fila con el numero de partidas ganadas del jugador con el id que estamos analizando
			ganadas_jugadori = mysql_store_result (conn);
			row_ganadas = mysql_fetch_row (ganadas_jugadori);
			if (row_ganadas != NULL){
				//printf("JUGADOR: %s HA GANADO  %s PARTIDAS\n", row_id_j[0], row_ganadas[0]);
				int ganadas = atoi(row_ganadas[0]);
				if (ganadas > record)
				{
					// Si paretidas ganadas de este jugador es mayor al record que teniamos, todo cambia a este nuevo record, 
					// entonces el resultado estara escrito en base a este record y hay que borrar todo lo que habia escrito anteriormente.
					record = ganadas;
					sprintf(resultado, "%d*%s/",ganadas,row_id_j[0]);		// Escribiremos record / mejor jugador /
				}
				// Si las partidas ganadas de este jugador coinciden con las de otro e iguala el record,este jugador tambien es mejor jugador
				else if(ganadas == record)
					sprintf(resultado, "%s%s/",resultado, row_id_j[0]);		//Escribiremos record/ mejor_jugador1 / mejor_jugador2 / ....

			}
			// obtenemos la siguiente fila de los id de jugadores
			row_id_j = mysql_fetch_row (id_jugadores);
		}
		
		resultado[strlen(resultado)-1] = ' '; // Quitamos la ultima barra del final para que no moleste
		//printf("%s\n", resultado);
		
		char res[200];
		char *p1;
		
		// A partir del resultado que ha quedado de la forma Record*Jugador1/Jugador2/Jugador3... y hasta todos los jugadores que igualen el record
		// Hacemos el print por consola.
		strcpy(res, resultado);
		p1 = strtok(res,"*");
		int record_total = atoi(p1);
		printf("\n\nRECORD DE PARTIDAS GANADAS = %d\n",record_total);
		printf("----------------------ID MEJORES JUGADORES----------------------\n");

		p1 = strtok(NULL, "/");
		while (p1!=NULL)
		{
			int id_jugador = atoi(p1);
			printf("Jugador %d\n", id_jugador);
			p1 = strtok(NULL, "/");
		}
	}
		
		mysql_close (conn);
		exit(0);

}
