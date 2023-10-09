/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>

/**
 * @brief Escalona um processo para execução.
 *
 * @param proc Processo a ser escalonado.
 */
PUBLIC void sched(struct process *proc)
{
    proc->state = PROC_READY;
    proc->counter = 0;

    // Define a prioridade com base nos atributos do processo (você pode personalizar esta lógica).
    set_priority(proc);
}

/**
 * @brief Interrompe o processo em execução.
 */
PUBLIC void stop(void)
{
    curr_proc->state = PROC_STOPPED;
    sndsig(curr_proc->father, SIGCHLD);
    yield();
}

/**
 * @brief Retoma a execução de um processo.
 *
 * @param proc Processo a ser retomado.
 *
 * @note O processo deve estar parado para ser retomado.
 */
PUBLIC void resume(struct process *proc)
{
    /* Retoma somente se o processo estiver parado. */
    if (proc->state == PROC_STOPPED)
        sched(proc);
}

/**
 * @brief Libera o processador.
 */
PUBLIC void yield(void)
{
    struct process *p;    /* Processo em execução.     */
    struct process *next; /* Próximo processo a ser executado. */

    /* Re-escalonar o processo para execução. */
    if (curr_proc->state == PROC_RUNNING)
        sched(curr_proc);

    /* Lembre-se deste processo. */
    last_proc = curr_proc;

    /* Verifica o alarme. */
    for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
        /* Ignora processos inválidos. */
        if (!IS_VALID(p))
            continue;

        /* O alarme expirou. */
        if ((p->alarm) && (p->alarm < ticks))
            p->alarm = 0, sndsig(p, SIGALRM);
    }

    /* Escolha o próximo processo a ser executado. */
    next = IDLE;
    for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
        /* Ignora processos não prontos. */
        if (p->state != PROC_READY)
            continue;

        /*
         * Processo com
         * prioridade mais alta encontrado.
         */
        if (p->priority > next->priority)
        {
            next->counter++;
            next = p;
        }
        else
        {
            p->counter++;
        }
    }

    /* Troca para o próximo processo. */
    next->priority = PRIO_USER;
    next->state = PROC_RUNNING;
    next->counter = PROC_QUANTUM;
    if (curr_proc != next)
        switch_to(next);
}
