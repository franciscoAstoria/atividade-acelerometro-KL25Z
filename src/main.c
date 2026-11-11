#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#define PERIODO_MS 50

static const struct device *const acelerometro =
    DEVICE_DT_GET(DT_ALIAS(accel0));

static volatile int etapa = -1;
static volatile uint32_t tempo_ms = 0;
static volatile double aceleracao[3];
static volatile double velocidade[3];
static volatile double posicao[3];

static void tarefa_aquisicao(void *p1, void *p2, void *p3)
{
    (void)p1;
    (void)p2;
    (void)p3;

    struct sensor_value eixos[3];
    uint32_t contador_ms = 0;

    while (1) {
        if (etapa == 0) {
            int ret = sensor_sample_fetch(acelerometro);
            if (ret == 0) {
                ret = sensor_channel_get(acelerometro, SENSOR_CHAN_ACCEL_XYZ,
                                         eixos);
            }

            if (ret == 0) {
                for (int eixo = 0; eixo < 3; eixo++) {
                    aceleracao[eixo] = sensor_value_to_double(&eixos[eixo]);
                }
                tempo_ms = contador_ms;
                etapa = 1;
            } else {
                printk("# ERRO: leitura do acelerometro (%d).\n", ret);
            }

            k_msleep(PERIODO_MS);
            contador_ms += PERIODO_MS;
        } else {
            k_msleep(1);
        }
    }
}

static void tarefa_calculo(void *p1, void *p2, void *p3)
{
    (void)p1;
    (void)p2;
    (void)p3;

    double aceleracao_anterior[3] = {0};
    uint32_t tempo_anterior_ms = 0;
    int primeira_amostra = 1;

    while (1) {
        if (etapa == 1) {
            if (!primeira_amostra) {
                double dt = (tempo_ms - tempo_anterior_ms) / 1000.0;

                for (int eixo = 0; eixo < 3; eixo++) {
                    double velocidade_anterior = velocidade[eixo];

                    velocidade[eixo] +=
                        (aceleracao_anterior[eixo] + aceleracao[eixo]) * dt / 2.0;
                    posicao[eixo] +=
                        (velocidade_anterior + velocidade[eixo]) * dt / 2.0;
                }
            }

            for (int eixo = 0; eixo < 3; eixo++) {
                aceleracao_anterior[eixo] = aceleracao[eixo];
            }
            tempo_anterior_ms = tempo_ms;
            primeira_amostra = 0;
            etapa = 2;
        }
        k_msleep(1);
    }
}

static void tarefa_envio(void *p1, void *p2, void *p3)
{
    (void)p1;
    (void)p2;
    (void)p3;

    while (1) {
        if (etapa == 2) {
            printk("%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                   (unsigned int)tempo_ms,
                   aceleracao[0], aceleracao[1], aceleracao[2],
                   velocidade[0], velocidade[1], velocidade[2],
                   posicao[0], posicao[1], posicao[2]);
            etapa = 0;
        }
        k_msleep(1);
    }
}

K_THREAD_DEFINE(thread_aquisicao, 1024, tarefa_aquisicao,
                NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread_calculo, 1024, tarefa_calculo,
                NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(thread_envio, 1536, tarefa_envio,
                NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    const struct sensor_value frequencia = {.val1 = 100, .val2 = 0};

    k_msleep(1500);
    if (!device_is_ready(acelerometro)) {
        printk("# ERRO: acelerometro nao esta pronto.\n");
        return 0;
    }

    int ret = sensor_attr_set(acelerometro, SENSOR_CHAN_ALL,
                              SENSOR_ATTR_SAMPLING_FREQUENCY, &frequencia);
    if (ret < 0) {
        printk("# ERRO: configuracao da frequencia (%d).\n", ret);
        return 0;
    }
    k_msleep(100);

    printk("# Aceleracao em m/s^2, velocidade em m/s, posicao em m.\n");
    printk("# Os valores de aceleracao incluem a gravidade.\n");
    printk("t_ms,ax,ay,az,vx,vy,vz,px,py,pz\n");
    etapa = 0;
    return 0;
}
