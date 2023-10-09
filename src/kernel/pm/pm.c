/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *              2016-2016 Subhra S. Sarkar <rurtle.coder@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */
#include <nanvix/config.h>
#include <nanvix/const.h>
#include <nanvix/dev.h>
#include <nanvix/fs.h>
#include <nanvix/hal.h>
#include <nanvix/mm.h>
#include <nanvix/pm.h>
#include <nanvix/klib.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>

/**
 * @brief Diretório de páginas do processo ocioso.
 */
EXTERN struct pde idle_pgdir[];

/**
 * @brief Stack do kernel do processo ocioso.
 */
PUBLIC char idle_kstack[KSTACK_SIZE];

/**
 * @brief Tabela de processos.
 */
PUBLIC struct process proctab[PROC_MAX];

/**
 * @brief Processo atual em execução.
 */
PUBLIC struct process *curr_proc = IDLE;

/**
 * @brief Último processo em execução.
 */
PUBLIC struct process *last_proc = IDLE;

/**
 * @brief Próximo ID de processo disponível.
 */
PUBLIC pid_t next_pid = 0;

/**
 * @brief Número atual de processos no sistema.
 */
PUBLIC unsigned nprocs = 0;

/* Define constantes para o cálculo de prioridade. */
#define PRIOMIN 0
#define DELTAPRIO 50

/* Contador de posição. */
PRIVATE int position_counter = 0;

/**
 * @brief Inicializa o sistema de gerenciamento de processos.
 */
PUBLIC void pm_init(void)
{
    int i;             /* Índice do loop. */
    struct process *p; /* Processo de trabalho. */

    /* Inicializa a tabela de processos. */
    for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
        p->flags = 0;
        p->state = PROC_DEAD;
    }

    /* Inicialização do processo de ocioso. */
    IDLE->cr3 = (dword_t)idle_pgdir;
    IDLE->intlvl = 1;
    IDLE->flags = 0;
    IDLE->received = 0;
    IDLE->kstack = idle_kstack;
    IDLE->restorer = NULL;
    for (i = 0; i < NR_SIGNALS; i++)
        IDLE->handlers[i] = SIG_DFL;
    IDLE->irqlvl = INT_LVL_5;
    IDLE->pgdir = idle_pgdir;
    for (i = 0; i < NR_PREGIONS; i++)
        IDLE->pregs[i].reg = NULL;
    IDLE->size = 0;
    for (i = 0; i < OPEN_MAX; i++)
        IDLE->ofiles[i] = NULL;
    IDLE->close = 0;
    IDLE->umask = S_IXUSR | S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH;
    IDLE->tty = NULL_DEV;
    IDLE->status = 0;
    IDLE->nchildren = 0;
    IDLE->uid = SUPERUSER;
    IDLE->euid = SUPERUSER;
    IDLE->suid = SUPERUSER;
    IDLE->gid = SUPERGROUP;
    IDLE->egid = SUPERGROUP;
    IDLE->sgid = SUPERGROUP;
    IDLE->pid = next_pid++;
    IDLE->pgrp = IDLE;
    IDLE->father = NULL;
    kstrncpy(IDLE->name, "idle", NAME_MAX);
    IDLE->utime = 0;
    IDLE->ktime = 0;
    IDLE->cutime = 0;
    IDLE->cktime = 0;
    IDLE->state = PROC_RUNNING;
    IDLE->counter = PROC_QUANTUM;
    IDLE->nice = NZERO;
    IDLE->alarm = 0;
    IDLE->next = NULL;
    IDLE->chain = NULL;
    set_priority(IDLE); // Define a prioridade para o processo IDLE.

    nprocs++;

    enable_interrupts();
}

/**
 * @brief Define a prioridade de um processo com base em sua posição na fila de pronto.
 *
 * @param proc Processo para atualizar a prioridade.
 */
PUBLIC void set_priority(struct process *proc)
{
    /* Calcula a prioridade com base na posição na fila de pronto. */
    proc->priority = PRIOMIN + DELTAPRIO * proc->position;
}

/**
 * @brief Escalona um processo para execução.
 *
 * @param proc Processo a ser escalonado.
 */
PUBLIC void sched(struct process *proc)
{
    proc->state = PROC_READY;
    proc->position = position_counter++; // Atribui a posição com base na ordem de chegada.
}
