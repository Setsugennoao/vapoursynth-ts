import { VS_MAKE_VIDEO_ID } from './utils'

export { Core, Plugin, VideoNode } from './types/core'

export enum MediaType {
    VIDEO = 1,
    AUDIO = 2,
}

export enum ColorFamily {
    UNDEFINED = 0,
    GRAY = 1,
    RGB = 2,
    YUV = 3,
}

export const { UNDEFINED, GRAY, RGB, YUV } = ColorFamily

export enum SampleType {
    INTEGER = 0,
    FLOAT = 1,
}

export const { INTEGER, FLOAT } = SampleType

export enum PresetFormat {
    NONE = 0,
    GRAY8 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 8, 0, 0),
    GRAY9 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 9, 0, 0),
    GRAY10 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 10, 0, 0),
    GRAY12 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 12, 0, 0),
    GRAY14 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 14, 0, 0),
    GRAY16 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 16, 0, 0),
    GRAY32 = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.INTEGER, 32, 0, 0),
    GRAYH = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.FLOAT, 16, 0, 0),
    GRAYS = VS_MAKE_VIDEO_ID(ColorFamily.GRAY, SampleType.FLOAT, 32, 0, 0),
    YUV410P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 2, 2),
    YUV411P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 2, 0),
    YUV440P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 0, 1),
    YUV420P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 1, 1),
    YUV422P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 1, 0),
    YUV444P8 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 8, 0, 0),
    YUV420P9 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 9, 1, 1),
    YUV422P9 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 9, 1, 0),
    YUV444P9 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 9, 0, 0),
    YUV420P10 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 10, 1, 1),
    YUV422P10 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 10, 1, 0),
    YUV444P10 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 10, 0, 0),
    YUV420P12 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 12, 1, 1),
    YUV422P12 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 12, 1, 0),
    YUV444P12 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 12, 0, 0),
    YUV420P14 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 14, 1, 1),
    YUV422P14 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 14, 1, 0),
    YUV444P14 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 14, 0, 0),
    YUV420P16 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 16, 1, 1),
    YUV422P16 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 16, 1, 0),
    YUV444P16 = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.INTEGER, 16, 0, 0),
    YUV444PH = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.FLOAT, 16, 0, 0),
    YUV444PS = VS_MAKE_VIDEO_ID(ColorFamily.YUV, SampleType.FLOAT, 32, 0, 0),
    RGB24 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 8, 0, 0),
    RGB27 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 9, 0, 0),
    RGB30 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 10, 0, 0),
    RGB36 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 12, 0, 0),
    RGB42 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 14, 0, 0),
    RGB48 = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.INTEGER, 16, 0, 0),
    RGBH = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.FLOAT, 16, 0, 0),
    RGBS = VS_MAKE_VIDEO_ID(ColorFamily.RGB, SampleType.FLOAT, 32, 0, 0),
}

export const {
    NONE,
    GRAY8,
    GRAY9,
    GRAY10,
    GRAY12,
    GRAY14,
    GRAY16,
    GRAY32,
    GRAYH,
    GRAYS,
    YUV410P8,
    YUV411P8,
    YUV440P8,
    YUV420P8,
    YUV422P8,
    YUV444P8,
    YUV420P9,
    YUV422P9,
    YUV444P9,
    YUV420P10,
    YUV422P10,
    YUV444P10,
    YUV420P12,
    YUV422P12,
    YUV444P12,
    YUV420P14,
    YUV422P14,
    YUV444P14,
    YUV420P16,
    YUV422P16,
    YUV444P16,
    YUV444PH,
    YUV444PS,
    RGB24,
    RGB27,
    RGB30,
    RGB36,
    RGB42,
    RGB48,
    RGBH,
    RGBS,
} = PresetFormat

export enum ColorRange {
    FULL = 0,
    LIMITED = 1,
}

export const { FULL: RANGE_FULL, LIMITED: RANGE_LIMITED } = ColorRange

export enum ChromaLocation {
    LEFT = 0,
    CENTER = 1,
    TOP_LEFT = 2,
    TOP = 3,
    BOTTOM_LEFT = 4,
    BOTTOM = 5,
}

export const {
    LEFT: CHROMA_LEFT,
    CENTER: CHROMA_CENTER,
    TOP_LEFT: CHROMA_TOP_LEFT,
    TOP: CHROMA_TOP,
    BOTTOM_LEFT: CHROMA_BOTTOM_LEFT,
    BOTTOM: CHROMA_BOTTOM,
} = ChromaLocation

export enum FieldBased {
    PROGRESSIVE = 0,
    BOTTOM = 1,
    TOP = 2,
}

export const { PROGRESSIVE: FIELD_PROGRESSIVE, BOTTOM: FIELD_BOTTOM, TOP: FIELD_TOP } = FieldBased

export enum MatrixCoefficients {
    RGB = 0,
    BT709 = 1,
    UNSPECIFIED = 2,
    FCC = 4,
    BT470_BG = 5,
    ST170_M = 6 /* Equivalent to 5. */,
    ST240_M = 7,
    YCGCO = 8,
    BT2020_NCL = 9,
    BT2020_CL = 10,
    CHROMATICITY_DERIVED_NCL = 12,
    CHROMATICITY_DERIVED_CL = 13,
    ICTCP = 14,
}

export const {
    RGB: MATRIX_RGB,
    BT709: MATRIX_BT709,
    UNSPECIFIED: MATRIX_UNSPECIFIED,
    FCC: MATRIX_FCC,
    BT470_BG: MATRIX_BT470_BG,
    ST170_M: MATRIX_ST170_M,
    ST240_M: MATRIX_ST240_M,
    YCGCO: MATRIX_YCGCO,
    BT2020_NCL: MATRIX_BT2020_NCL,
    BT2020_CL: MATRIX_BT2020_CL,
    CHROMATICITY_DERIVED_NCL: MATRIX_CHROMATICITY_DERIVED_NCL,
    CHROMATICITY_DERIVED_CL: MATRIX_CHROMATICITY_DERIVED_CL,
    ICTCP: MATRIX_ICTCP,
} = MatrixCoefficients

export enum TransferCharacteristics {
    BT709 = 1,
    UNSPECIFIED = 2,
    BT470_M = 4,
    BT470_BG = 5,
    BT601 = 6 /* Equivalent to 1. */,
    ST240_M = 7,
    LINEAR = 8,
    LOG_100 = 9,
    LOG_316 = 10,
    IEC_61966_2_4 = 11,
    IEC_61966_2_1 = 13,
    BT2020_10 = 14 /* Equivalent to 1. */,
    BT2020_12 = 15 /* Equivalent to 1. */,
    ST2084 = 16,
    ARIB_B67 = 18,
}

export const {
    BT709: TRANSFER_BT709,
    UNSPECIFIED: TRANSFER_UNSPECIFIED,
    BT470_M: TRANSFER_BT470_M,
    BT470_BG: TRANSFER_BT470_BG,
    BT601: TRANSFER_BT601,
    ST240_M: TRANSFER_ST240_M,
    LINEAR: TRANSFER_LINEAR,
    LOG_100: TRANSFER_LOG_100,
    LOG_316: TRANSFER_LOG_316,
    IEC_61966_2_4: TRANSFER_IEC_61966_2_4,
    IEC_61966_2_1: TRANSFER_IEC_61966_2_1,
    BT2020_10: TRANSFER_BT2020_10,
    BT2020_12: TRANSFER_BT2020_12,
    ST2084: TRANSFER_ST2084,
    ARIB_B67: TRANSFER_ARIB_B67,
} = TransferCharacteristics

export enum ColorPrimaries {
    BT709 = 1,
    UNSPECIFIED = 2,
    BT470_M = 4,
    BT470_BG = 5,
    ST170_M = 6,
    ST240_M = 7 /* Equivalent to 6. */,
    FILM = 8,
    BT2020 = 9,
    ST428 = 10,
    ST431_2 = 11,
    ST432_1 = 12,
    EBU3213_E = 22,
}

export const {
    BT709: PRIMARIES_BT709,
    UNSPECIFIED: PRIMARIES_UNSPECIFIED,
    BT470_M: PRIMARIES_BT470_M,
    BT470_BG: PRIMARIES_BT470_BG,
    ST170_M: PRIMARIES_ST170_M,
    ST240_M: PRIMARIES_ST240_M,
    FILM: PRIMARIES_FILM,
    BT2020: PRIMARIES_BT2020,
    ST428: PRIMARIES_ST428,
    ST431_2: PRIMARIES_ST431_2,
    ST432_1: PRIMARIES_ST432_1,
    EBU3213_E: PRIMARIES_EBU3213_E,
} = ColorPrimaries

export enum CoreCreationFlags {
    None = 0,
    EnableGraphInspection = 1,
    DisableAutoLoading = 2,
    DisableLibraryUnloading = 4,
}
