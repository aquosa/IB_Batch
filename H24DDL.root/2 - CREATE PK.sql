USE [DTN_INTERFINANZAS]
GO


/****** Object:  Index [PK_EXTCTA]    Script Date: 12/04/2017 9:18:17 ******/
ALTER TABLE [dbo].[EXTCTA] ADD  CONSTRAINT [PK_EXTCTA] PRIMARY KEY CLUSTERED 
(
	[EX_TIPCTA] ASC,
	[EX_CUENTA] ASC,
	[EX_NROLOG] ASC,
	[EX_FECOPE] ASC,
	[EX_MARCADIFERIDO] ASC,
	[EX_CORTOS] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 80) ON [PRIMARY]
GO


