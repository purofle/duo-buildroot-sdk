// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for ES8328 on CVITEK CV1835
 *
 * Copyright 2023 Purofle
 *
 * Author: Purofle
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/io.h>
#include <linux/proc_fs.h>

#include "../codecs/es8328.h"

struct card_private
{
    int tmp;
};

static int cv1835_es8328_hw_params(struct snd_pcm_substream *substream,
                                   struct snd_pcm_hw_params *params)
{
    return 0;
}

static struct snd_soc_ops cv1835_es8328_ops = {
    .hw_params = cv1835_es8328_hw_params,
};

static int cv1835_es8328_codec_init(struct snd_soc_pcm_runtime *rtd)
{
    return 0;
}

SND_SOC_DAILINK_DEFS(dailink_es8328,
                     DAILINK_COMP_ARRAY(COMP_CPU("4120000.i2s")),
                     DAILINK_COMP_ARRAY(COMP_CODEC("es8328", "es8328-hifi-analog")),
                     DAILINK_COMP_ARRAY(COMP_PLATFORM("4120000.i2s")));

static struct snd_soc_dai_link cv1835_es8328_dai[] = {
    {.name = "es8328",
     .stream_name = "es8328_i2s",
     .init = cv1835_es8328_codec_init,
     .ops = &cv1835_es8328_ops,
     .dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
                SND_SOC_DAIFMT_CBS_CFS,
     SND_SOC_DAILINK_REG(dailink_es8328)}};

static struct snd_soc_card cv1835_es8328 = {
    .owner = THIS_MODULE,
    .dai_link = cv1835_es8328_dai,
    .num_links = 1,
};

static int cv1835_es8328_probe(struct platform_device *pdev)
{
    struct snd_soc_card *card = &cv1835_es8328;
    int ret;

    struct card_private *ctx;

    ctx = devm_kzalloc(&pdev->dev, sizeof(*ctx), GFP_KERNEL);
    if (!ctx)
        return -ENOMEM;

    card->dev = &pdev->dev;
    snd_soc_card_set_drvdata(card, ctx); // save card info to snd_card

    ret = devm_snd_soc_register_card(&pdev->dev, card);
    if (ret)
    {
        dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n",
                ret);
        return ret;
    }

    return 0;
}

static const struct of_device_id cvi_es8328_audio_match_ids[] = {
    {
        .compatible = "cvitek,cv1835-es8328",
    },
    {}};
MODULE_DEVICE_TABLE(of, cvi_es8328_audio_match_ids);

static struct platform_driver cv1835_es8328_driver = {
    .driver = {
        .name = "cv1835-es8328",
        .pm = &snd_soc_pm_ops,
        .of_match_table = cvi_es8328_audio_match_ids,
    },
    .probe = cv1835_es8328_probe,
};

module_platform_driver(cv1835_es8328_driver);

MODULE_AUTHOR("Purofle");
MODULE_DESCRIPTION("ALSA SoC cv1835 es8328 driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-es8328");
