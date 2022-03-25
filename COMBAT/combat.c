#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

const float FPS = 60;

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const float THETA = M_PI/4;
const float RAIO_CAMPO_FORCA = 50;

const float VEL_TANQUE = 5.0;
const float PASSO_ANGULO = M_PI/60;

const float RAIO_TIRO = 15;
const float VELOCIDADE_TIRO =0.3;

typedef struct Ponto{
  float x,y;
}Ponto;

typedef struct Tanque{
  Ponto centro;
  Ponto A, B, C;
  ALLEGRO_COLOR cor;
  float vel;
  float angulo;
  float x_comp, y_comp;
  float vel_angular;
  int id;
  }Tanque;

 typedef struct Tiro {
   Ponto centro;
   ALLEGRO_COLOR cor;
   Ponto move;
   float raio;
   bool existe;
 }Tiro;

 typedef struct Bloco{
   Ponto sup_esq, inf_dir;
   ALLEGRO_COLOR cor;
 }Bloco;

//cria o layout do campo de batalha

void desenhaCenario(){
  al_clear_to_color(al_map_rgb(0, 0, 0));
}



void inicializaBloco(Bloco *b1, Bloco *b2){
  //inicializa bloco 1
  b1->sup_esq.x = 173;
  b1->sup_esq.y = 108;
  b1->inf_dir.x = 205;
  b1->inf_dir.y = 432;
//  b1->cor = al_map_rgb(rand()%120, rand()%256, rand()%256);
  //inicializa bloco 2
  b2->sup_esq.x = 768;
  b2->sup_esq.y = 108;
  b2->inf_dir.x = 800;
  b2->inf_dir.y = 432;
//  b2->cor = al_map_rgb(rand()%120, rand()%256, rand()%256);

}

void desenhaBloco(Bloco b1){
  //desenha o bloco em questão
  b1.cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
  al_draw_filled_rectangle (b1.sup_esq.x, b1.sup_esq.y, b1.inf_dir.x, b1.inf_dir.y, b1.cor);

}



//inicializa Tanque
void initTanque(Tanque *t){

  if (t->id == 1){
    t->centro.x = SCREEN_W/8;
    t->centro.y = SCREEN_H/2;
  }
  else if (t->id == 2){
    t->centro.x = SCREEN_W/1.125;
    t->centro.y = SCREEN_H/2;
  }
  t->cor = al_map_rgb(rand()%120, rand()%256, rand()%256);

  t->A.x = 0;
  t->A.y = -RAIO_CAMPO_FORCA;

  float alpha = M_PI/2 - THETA;
  float h = RAIO_CAMPO_FORCA * sin(alpha);
  float w = RAIO_CAMPO_FORCA * sin(THETA);

  t->B.x = -w;
  t->B.y = h;

  t->C.x = w;
  t->C.y = h;

  t->vel = 0;
  t->angulo = M_PI/2;
  t->x_comp = cos(t->angulo);
  t->y_comp = sin(t->angulo);

  t->vel_angular = 0;
}



float distanciaEntrePontos(Ponto p1, Ponto p2){
  return sqrt(pow(p2.x-p1.x, 2) + pow(p2.y-p1.y, 2));
}
//aqui verificamos as condições em que o tanque não pode se mexer.
int bloqueiaTanque(Tanque t){
  //verifica colisão no campo superior da tela
  if (t.centro.y < RAIO_CAMPO_FORCA){
    return 1;
  }
  //verifica colisão na lateral esquerda da tela
  else if (t.centro.x <RAIO_CAMPO_FORCA){
    return 2;
  }
  //verifica colisão no campo inferior da tela
  else if (t.centro.y + RAIO_CAMPO_FORCA > SCREEN_H){
    return 3;
  }
  //verifica colisão na lateral direita da tela
  else if (t.centro.x + RAIO_CAMPO_FORCA > SCREEN_W){
    return 4;
  }

  else return 0;
}

void desenhaTanque(Tanque t) {

    al_draw_circle(t.centro.x, t.centro.y, RAIO_CAMPO_FORCA, t.cor, 1);

    al_draw_filled_triangle(t.A.x + t.centro.x, t.A.y + t.centro.y,
                            t.B.x + t.centro.x, t.B.y + t.centro.y,
                            t.C.x + t.centro.x, t.C.y + t.centro.y,
                            t.cor);

}

void Rotate(Ponto *p, float angulo){
  float x = p->x, y = p->y;

  p->x=(x * cos(angulo)) - (y * sin(angulo));
  p->y=(y * cos(angulo)) + (x * sin(angulo));
}

void rotacionaTanque(Tanque *t){

  if (t -> vel_angular != 0){

    Rotate(&t->A, t->vel_angular);
    Rotate(&t->B, t->vel_angular);
    Rotate(&t->C, t->vel_angular);

    t->angulo += t->vel_angular;
    t->x_comp = cos(t->angulo);
    t->y_comp = sin(t->angulo);
  }
}

int colisaoEntreTanques(Tanque t1, Tanque t2){
    if (distanciaEntrePontos(t1.centro, t2.centro) <= RAIO_CAMPO_FORCA*2){
      return 1;
    }
    else return 0;
}

int colisaoBlocoCirculo(Ponto t, Bloco b, float raio){
  Ponto sup_dir, inf_esq;
  sup_dir.x = b.inf_dir.x;
  sup_dir.y = b.sup_esq.y;
  inf_esq.x = b.sup_esq.x;
  inf_esq.y = b.inf_dir.y;
  //verifica se o tanque colide com os blocos na parte do lado(vertical)
  if (t.y >= b.sup_esq.y && t.y <= b.inf_dir.y){
    if (t.x + raio >= b.sup_esq.x && t.x - raio <= b.inf_dir.x){
        return 1;
    }
  }
  //verifica se o tanque colide com a  parte de cima ou de baixo dos blocos (horizontal)
  else if(t.x >= b.sup_esq.x && t.x <= b.inf_dir.x){
    if(t.y + raio >= b.sup_esq.y && t.y - raio <= b.inf_dir.y){
        return 2;
    }
  }
  //verificamos se o tanque colide com algum dos vértices(quinas)
  else if(distanciaEntrePontos(t, b.sup_esq) <= raio){
    return 3;
  }

  else if(distanciaEntrePontos(t, sup_dir) <= raio){
    return 3;
  }

  else if(distanciaEntrePontos(t, inf_esq) <= raio){
    return 3;
  }

  else if(distanciaEntrePontos(t, b.inf_dir) <= raio){
    return 3;
  }
 return 0;
}

void afastaTanqueBloco(Tanque* t, Bloco b, int cb1t){
  switch (cb1t) {
    case 1:
    t->centro.y += t->vel * t->y_comp;
    t->centro.x -= 5 * (t->vel * t->x_comp);
    break;


    case 2:
    t->centro.y -= 5 * t->vel * t->y_comp;
    t->centro.x += t->vel * t->x_comp;
    break;

    case 3:
    t->centro.y -= 8 * t->vel * t->y_comp;
    t->centro.x -= 8 * t->vel * t->x_comp;
    break;

  }
}

void atualizaTanque(Tanque *t, Tanque t2, Bloco b1, Bloco b2){
  int cb1t = colisaoBlocoCirculo(t->centro, b1, RAIO_CAMPO_FORCA);
  int cb2t = colisaoBlocoCirculo(t->centro, b2, RAIO_CAMPO_FORCA);

  int k = bloqueiaTanque(*t);
  rotacionaTanque(t);

    if (k == 0 && colisaoEntreTanques(*t, t2) == 0 && cb1t == 0 && cb2t == 0){
      t->centro.y += t->vel * t->y_comp;
      t->centro.x += t->vel * t->x_comp;
    }

    else if (colisaoEntreTanques(*t, t2) == 1){
      t->centro.y -= (8 * t->vel * t->y_comp);
      t->centro.x -= (8* t->vel * t->x_comp);
    }

    else if (cb1t != 0){
      afastaTanqueBloco(t, b1, cb1t);
    }

    else if (cb2t != 0){
      afastaTanqueBloco(t, b2, cb2t);
    }

    else if (k != 0){
      switch (k) {
        case 1:
        t->centro.y = RAIO_CAMPO_FORCA;
        t->centro.x += t->vel * t->x_comp;
        break;

        case 2:
        t->centro.y += t->vel * t->y_comp;
        t->centro.x = RAIO_CAMPO_FORCA;
        break;

        case 3:
        t->centro.y = SCREEN_H - RAIO_CAMPO_FORCA;
        t->centro.x += t->vel * t->x_comp;
        break;

        case 4:
        t->centro.y += t->vel * t->y_comp;
        t->centro.x = SCREEN_W - RAIO_CAMPO_FORCA;
        break;
      }
    }

}

void disparo(Tanque t, Tiro *tiro, bool libera){

  if (libera == true || tiro->existe == false){
    tiro->existe = true;
    tiro->cor = al_map_rgb(255, 255, 255);
    tiro->centro.x = t.A.x + t.centro.x;
    tiro->centro.y = t.A.y + t.centro.y;
    tiro->move.x = VELOCIDADE_TIRO * t.A.x;
    tiro->move.y = VELOCIDADE_TIRO * t.A.y;
    tiro->raio = 5;
  }
}

void moveDisparo(Tiro *tiro){
  tiro->centro.x += tiro->move.x;
  tiro->centro.y += tiro->move.y;
}

void desenhaDisparo(Tiro *tiro) {
	al_draw_filled_circle(tiro->centro.x, tiro->centro.y, tiro->raio, tiro->cor);
}

//esta função faz com que o tiro desapareça
void someDisparo(Tiro *tiro){
  tiro->existe = false;
  tiro->cor = al_map_rgb(0, 0, 0);
  tiro->move.x = 0;
  tiro->move.y = 0;
  tiro->raio = 0;
}

//esta função verifica se o tiro ainda está dentro da tela;
int tiroPelaCulatra(Tiro t){
  if (t.existe == true && t.centro.y >= t.raio && t.centro.x >= t.raio && t.centro.y + t.raio <= SCREEN_H && t.centro.x + t.raio <= SCREEN_W){
  return 0;
  }
  else return 1;
}


//esta função faz as verificações necessárias para garantir que em caso de colisão ou excessão dos limites da tela, o tiro desapareça
void verificaDisparo(Tiro *tiro, Tanque t2, int *score, bool* libera, Bloco b1, Bloco b2){


  if (distanciaEntrePontos(tiro->centro, t2.centro) < tiro->raio + RAIO_CAMPO_FORCA){
    if (tiro->raio > 0){
    *score += 1;
    }
    someDisparo(tiro);
    *libera = true;
    tiro->existe = false;

  }

  if (tiroPelaCulatra(*tiro)){
    *libera = true;
    someDisparo(tiro);
  }

  else {
  //tiro->cor =  al_map_rgb(255, 0, 0);
   *libera = false;
    tiro->existe = true;

  }



   if (colisaoBlocoCirculo(tiro->centro, b1, tiro->raio) != 0 || colisaoBlocoCirculo(tiro->centro, b2, tiro->raio) != 0){
    someDisparo(tiro);

  }
}

int verificaScore(int score1, int score2){
  if (score1 == 5) {
    return 1;
  }
  else if (score2 == 5){
    return 2;
  }
  else {
    return 0;
  }
}

void imprimeHistorico(int winner){
  ALLEGRO_FONT* font = al_load_font("allegro-regular-1.ttf",40,0);
  time_t currentTime;
  time(&currentTime);
  char venceu[9];
  if (winner == 1){
    strcpy(venceu, "Player 1");
  }
  else if (winner == 2) {
    strcpy(venceu, "Player 2");
  }

  else {
    strcpy(venceu, "None");
  }
    FILE *arq;
    arq = fopen("history.txt","a");
    fprintf(arq, "%s Winner: %s\n\n", ctime(&currentTime), venceu);
    fclose(arq);
    al_draw_textf(font,al_map_rgb(255,255,255),SCREEN_W/10,SCREEN_H/4,0," Winner: %s",venceu);
}

void mostraHistorico(void){
  ALLEGRO_FONT* font = al_load_font("allegro-regular-1.ttf",18,0);
  float x = SCREEN_W/1.5, y = SCREEN_H/8;
  char linha[50];
  FILE *arq;
  arq = fopen("history.txt", "r");
  while (!feof(arq)){
    fgets(linha, 50, arq);
    al_draw_textf(font,al_map_rgb(255,255,255),x,y,0,"%s",linha);
    y+=20;
  }
  fclose(arq);

}

int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	//----------------------- rotinas de inicializacao ---------------------------------------

	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
  //inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();
	ALLEGRO_FONT* font = al_load_font("allegro-regular-1.ttf",40,0);

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

  //cria o Tanque
  Tanque tanque_1, tanque_2;
  tanque_1.id = 1;
  tanque_2.id = 2;
  initTanque(&tanque_1);
  initTanque(&tanque_2);
  //cria o tiro;
  Tiro t1, t2;
  //cria o score dos jogadores (que descresce com o tempo)
  int score1 = 0, score2 = 0;

  //esta variável verifica se o tanque pode atirar
  bool liberaTiro1 = true;
  bool liberaTiro2 = true;

  //esta variável informa se os tiros existem (na tela)
  t1.existe = false;
  t2.existe = false;

  //aqui se criam os blocos
  Bloco b1, b2;

  int winner = 0;

  inicializaBloco(&b1, &b2);


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());



	//inicia o temporizador
	al_start_timer(timer);

	int playing = 1;



	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {


      winner = verificaScore(score1, score2);
      desenhaCenario();
      desenhaBloco(b1);
      desenhaBloco(b2);
      atualizaTanque(&tanque_1, tanque_2, b1, b2);
      desenhaTanque(tanque_1);
      atualizaTanque(&tanque_2, tanque_1, b1, b2);
      desenhaTanque(tanque_2);

      verificaDisparo(&t1, tanque_2, &score1, &liberaTiro1, b1, b2);
      verificaDisparo(&t2, tanque_1, &score2, &liberaTiro2, b1, b2);
      moveDisparo(&t1);
      desenhaDisparo(&t1);
      moveDisparo(&t2);
      desenhaDisparo(&t2);


//  verifica se algum acertou 5 tiros
      switch(winner){
        case 1:
        playing = 0;

        case 2:
        playing = 0;

      }

      //desenhando na tela os scores
    al_draw_textf(font,al_map_rgb(255,255,255),SCREEN_W/10,SCREEN_H/100,0,"score player 1: %d",score1);

    al_draw_textf(font,al_map_rgb(255,255,255),SCREEN_W/1.4,SCREEN_H/100,0,"score player 2: %d",score2);

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();


			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}

		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

      switch(ev.keyboard.keycode) {
        //controles do tanque_1
        case ALLEGRO_KEY_W:

          tanque_1.vel -= VEL_TANQUE;
          break;

        case ALLEGRO_KEY_S:

          tanque_1.vel += VEL_TANQUE;
          break;

        case ALLEGRO_KEY_D:

          tanque_1.vel_angular += PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_A:

          tanque_1.vel_angular -= PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_Q:

          disparo(tanque_1, &t1, liberaTiro1);
          break;

        //controles do tanque_2
        case ALLEGRO_KEY_UP:

          tanque_2.vel -= VEL_TANQUE;
          break;

        case ALLEGRO_KEY_DOWN:

          tanque_2.vel += VEL_TANQUE;
          break;

        case ALLEGRO_KEY_RIGHT:

          tanque_2.vel_angular += PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_LEFT:

          tanque_2.vel_angular -= PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_SPACE:

          disparo(tanque_2, &t2, liberaTiro2);
          break;

      }



    }
    else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
      //imprime qual tecla foi
      printf("\ncodigo tecla: %d", ev.keyboard.keycode);

      switch(ev.keyboard.keycode) {

        case ALLEGRO_KEY_W:

          tanque_1.vel += VEL_TANQUE;
          break;

        case ALLEGRO_KEY_S:

          tanque_1.vel -= VEL_TANQUE;
          break;

        case ALLEGRO_KEY_D:

          tanque_1.vel_angular -= PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_A:

          tanque_1.vel_angular += PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_Q:

          break;

        //controles do tanque_2
        case ALLEGRO_KEY_UP:

          tanque_2.vel += VEL_TANQUE;
          break;

        case ALLEGRO_KEY_DOWN:

          tanque_2.vel -= VEL_TANQUE;
          break;

        case ALLEGRO_KEY_RIGHT:

          tanque_2.vel_angular -= PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_LEFT:

          tanque_2.vel_angular += PASSO_ANGULO;
          break;

        case ALLEGRO_KEY_SPACE:

          break;

      }

    }

    //desenhando na tela os scores
/*  al_draw_textf(font,al_map_rgb(255,255,255),SCREEN_W/10,SCREEN_H/100,0,"score player 1: %d",score1);
  al_flip_display();
  al_draw_textf(font,al_map_rgb(255,255,255),SCREEN_W/1.4,SCREEN_H/100,0,"score player 2: %d",score2);
  al_flip_display();*/


	} //fim do while
 //colore a tela de preto
  al_clear_to_color(al_map_rgb(0,0,0));
  imprimeHistorico(winner);
  mostraHistorico();
  //reinicializa a tela
	al_flip_display();
    al_rest(3);

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
  al_destroy_font(font);


	return 0;
}
