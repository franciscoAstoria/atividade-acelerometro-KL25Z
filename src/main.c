/* Etapa 1: validar o acelerometro e a serial antes de separar as threads. */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#define PERIODO_MS 50

static const struct device *const acelerometro =
    DEVICE_DT_GET(DT_ALIAS(accel0));

int main(void)
{
    struct sensor_value eixos[3];
    const struct sensor_value frequencia = {.val1 = 100, .val2 = 0};
    uint32_t tempo_ms = 0;
    int ret;

    k_msleep(1500);
    if (!device_is_ready(acelerometro)) {
        printk("# ERRO: acelerometro nao esta pronto.\n");
        return 0;
    }

    ret = sensor_attr_set(acelerometro, SENSOR_CHAN_ALL,
                          SENSOR_ATTR_SAMPLING_FREQUENCY, &frequencia);
    if (ret < 0) {
        printk("# ERRO: configuracao da frequencia (%d).\n", ret);
        return 0;
    }
    k_msleep(100);

    printk("# MMA8451Q: aceleracao em m/s^2; pausa de 50 ms entre leituras.\n");
    printk("t_ms,ax,ay,az\n");

    while (1) {
        ret = sensor_sample_fetch(acelerometro);
        if (ret == 0) {
            ret = sensor_channel_get(acelerometro, SENSOR_CHAN_ACCEL_XYZ,
                                     eixos);
        }

        if (ret < 0) {
            printk("# ERRO: leitura do acelerometro (%d).\n", ret);
        } else {
            printk("%u,%.6f,%.6f,%.6f\n", (unsigned int)tempo_ms,
                   sensor_value_to_double(&eixos[0]),
                   sensor_value_to_double(&eixos[1]),
                   sensor_value_to_double(&eixos[2]));
        }

        k_msleep(PERIODO_MS);
        tempo_ms += PERIODO_MS;
    }
    return 0;
}
