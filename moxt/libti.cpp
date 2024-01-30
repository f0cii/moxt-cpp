#include "libti.hpp"
#include "tulipindicators/indicators.h"
#include <cstdio>

SEQ_FUNC void seq_test_ti() {
    /* Order data from oldest to newest (index 0 is oldest) */
    const double data_in[] = {5, 8, 12, 11, 9, 8, 7, 10, 11, 13};
    const int input_length =
        sizeof(data_in) / sizeof(double); /* 10 in this example */
    const double options[] = {3};
    /* Find start size for given options. */
    const int start = ti_sma_start(options);

    /* Output length is input length minus start size. */
    const int output_length = input_length - start;

    double *data_out = (double *)malloc(output_length * sizeof(double));
    assert(data_out != 0);

    const double *all_inputs[] = {data_in};
    double *all_outputs[] = {data_out};

    int error = ti_sma(input_length, all_inputs, options, all_outputs);
    assert(error == TI_OKAY);

    // 遍历所有指标
    /* Set info to first indicators in array. */
    const ti_indicator_info *info = ti_indicators;

    /* The last item is all zeros, so loop until then. */
    while (info->name != 0) {

        /* TODO use the info structure to do something.
        e.g. printf("Found %s\n", info->name); */

        if (info->type == TI_TYPE_OVERLAY)
            printf("type overlay ");
        else if (info->type == TI_TYPE_INDICATOR)
            printf("type indicator ");
        else if (info->type == TI_TYPE_MATH)
            printf("type math ");
        else if (info->type == TI_TYPE_SIMPLE)
            printf("type simple ");
        else
            printf("type unknown ");

        printf("name %s ", info->name);
        printf("full_name {%s} ", info->full_name);

        int i;
        printf("inputs {");
        for (i = 0; i < info->inputs; ++i)
            printf("%s%s", i ? " " : "", info->input_names[i]);
        printf("} ");

        printf("options {");
        for (i = 0; i < info->options; ++i)
            printf("%s{%s}", i ? " " : "", info->option_names[i]);
        printf("} ");

        printf("outputs {");
        for (i = 0; i < info->outputs; ++i)
            printf("%s{%s}", i ? " " : "", info->output_names[i]);
        printf("}");

        printf("\n");

        ++info; /* Next indicator. */
    }
}

SEQ_FUNC void seq_test_ti1() {
    /* Order data from oldest to newest (index 0 is oldest) */
    const double data_in[] = {5, 8, 12, 11, 9, 8, 7, 10, 11, 13};
    const int input_length =
        sizeof(data_in) / sizeof(double); /* 10 in this example */
    const double options[] = {3};
    /* Find start size for given options. */
    const int start = ti_sma_start(options);

    printf("input_length=%d\n", input_length);
    printf("start=%d\n", start);

    /* Output length is input length minus start size. */
    const int output_length = input_length - start;

    printf("output_length=%d\n", output_length);

    double *data_out = (double *)malloc(output_length * sizeof(double));
    assert(data_out != 0);

    const double *all_inputs[] = {data_in};
    double *all_outputs[] = {data_out};

    int error = ti_sma(input_length, all_inputs, options, all_outputs);
    assert(error == TI_OKAY);
}

SEQ_FUNC void seq_test_ti2(const double *data_in, const int input_length,
                           const double *options, double *data_out) {
    /* Order data from oldest to newest (index 0 is oldest) */
    // const double data_in[] = {5, 8, 12, 11, 9, 8, 7, 10, 11, 13};
    // const int input_length =
    //     sizeof(data_in) / sizeof(double); /* 10 in this example */
    // const double options[] = {3};
    /* Find start size for given options. */
    const int start = ti_sma_start(options);

    printf("input_length=%d\n", input_length);
    printf("start=%d\n", start);

    /* Output length is input length minus start size. */
    const int output_length = input_length - start;

    printf("output_length=%d\n", output_length);

    // double *data_out = malloc(output_length * sizeof(double));
    // assert(data_out != 0);

    const double *all_inputs[] = {data_in};
    double *all_outputs[] = {data_out};

    int error = ti_sma(input_length, all_inputs, options, all_outputs);
    assert(error == TI_OKAY);

    for (int i = 0; i < output_length; i++) {
        printf("%d=%f\n", i, data_out[i]);
    }
}

SEQ_FUNC void seq_test_ti3(const double **inputs, const int input_length,
                           const double *options, double **outputs) {
    /* Order data from oldest to newest (index 0 is oldest) */
    // const double data_in[] = {5, 8, 12, 11, 9, 8, 7, 10, 11, 13};
    // const int input_length =
    //     sizeof(data_in) / sizeof(double); /* 10 in this example */
    // const double options[] = {3};
    /* Find start size for given options. */
    const int start = ti_sma_start(options);

    // printf("input_length=%d\n", input_length);
    // printf("start=%d\n", start);

    /* Output length is input length minus start size. */
    const int output_length = input_length - start;

    // printf("output_length=%d\n", output_length);

    // double *data_out = malloc(output_length * sizeof(double));
    // assert(data_out != 0);

    // const double *all_inputs[] = {data_in};
    // double *all_outputs[] = {data_out};

    // int error = ti_sma(input_length, all_inputs, options, all_outputs);
    int error = ti_sma(input_length, inputs, options, outputs);
    assert(error == TI_OKAY);

    for (int i = 0; i < output_length; i++) {
        printf("%d=%f\n", i, outputs[0][i]);
    }
}

SEQ_FUNC const ti_indicator_info *seq_ti_get_first_indicator() {
    return ti_indicators;
}

SEQ_FUNC bool seq_ti_is_valid_indicator(const ti_indicator_info *info) {
    return info->name != 0;
}

SEQ_FUNC const ti_indicator_info *
seq_ti_get_next_indicator(const ti_indicator_info *info) {
    return ++info;
}

SEQ_FUNC const ti_indicator_info *seq_ti_find_indicator(const char *name) {
    return ti_find_indicator(name);
}

SEQ_FUNC const ti_indicator_info *seq_ti_indicator_at_index(int index) {
    return &ti_indicators[index];
}

SEQ_FUNC int seq_ti_indicator_start(const ti_indicator_info *info,
                                    TI_REAL *options) {
    return info->start(options);
}

SEQ_FUNC bool seq_ti_indicator_run(const ti_indicator_info *info,
                                    int input_size, TI_REAL **inputs,
                                    TI_REAL *options, TI_REAL **outputs) {
    auto res = info->indicator(input_size, inputs, options, outputs);
    return res == TI_OKAY;
}
