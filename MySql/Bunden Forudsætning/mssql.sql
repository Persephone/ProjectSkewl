/*
 Navicat Premium Data Transfer

 Source Server         : SQL Server
 Source Server Type    : SQL Server
 Source Server Version : 14001000
 Source Host           : localhost:1433
 Source Catalog        : skole
 Source Schema         : dbo

 Target Server Type    : SQL Server
 Target Server Version : 14001000
 File Encoding         : 65001

 Date: 03/04/2019 12:38:03
*/


-- ----------------------------
-- Table structure for accounts
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[accounts]') AND type IN ('U'))
	DROP TABLE [dbo].[accounts]
GO

CREATE TABLE [dbo].[accounts] (
  [Id] int  IDENTITY(1,1) NOT NULL,
  [Balance] decimal(30)  NOT NULL,
  [Posts] int DEFAULT ((0)) NOT NULL,
  [Created] datetime DEFAULT (getdate()) NOT NULL,
  [Deleted] datetime  NULL,
  [Owner] int DEFAULT ((0)) NOT NULL
)
GO

ALTER TABLE [dbo].[accounts] SET (LOCK_ESCALATION = TABLE)
GO


-- ----------------------------
-- Records of accounts
-- ----------------------------
SET IDENTITY_INSERT [dbo].[accounts] ON
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'2', N'16000', N'1', N'2019-04-03 12:21:51.993', NULL, N'3')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'3', N'20000', N'1', N'2019-04-03 12:22:06.920', NULL, N'5')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'5', N'25000', N'1', N'2019-04-03 12:23:20.477', NULL, N'1')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'6', N'200000', N'0', N'2019-04-03 12:23:46.380', NULL, N'1')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'8', N'300000', N'2', N'2019-04-03 12:23:58.880', NULL, N'3')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'11', N'200000', N'0', N'2019-04-03 12:26:09.427', NULL, N'1')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'13', N'200000', N'0', N'2019-04-03 12:26:19.647', NULL, N'1')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'17', N'200000', N'0', N'2019-04-03 12:29:38.910', NULL, N'1')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'19', N'200000', N'0', N'2019-04-03 12:29:47.447', NULL, N'4')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'20', N'0', N'2', N'2019-04-03 12:30:02.653', NULL, N'4')
GO

INSERT INTO [dbo].[accounts] ([Id], [Balance], [Posts], [Created], [Deleted], [Owner]) VALUES (N'28', N'400000', N'2', N'2019-04-03 12:30:34.360', NULL, N'4')
GO

SET IDENTITY_INSERT [dbo].[accounts] OFF
GO


-- ----------------------------
-- Table structure for customers
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[customers]') AND type IN ('U'))
	DROP TABLE [dbo].[customers]
GO

CREATE TABLE [dbo].[customers] (
  [Id] int  IDENTITY(1,1) NOT NULL,
  [Name] text COLLATE SQL_Latin1_General_CP1_CI_AS DEFAULT '' NOT NULL
)
GO

ALTER TABLE [dbo].[customers] SET (LOCK_ESCALATION = TABLE)
GO


-- ----------------------------
-- Records of customers
-- ----------------------------
SET IDENTITY_INSERT [dbo].[customers] ON
GO

INSERT INTO [dbo].[customers] ([Id], [Name]) VALUES (N'1', N'Bob')
GO

INSERT INTO [dbo].[customers] ([Id], [Name]) VALUES (N'2', N'Mark')
GO

INSERT INTO [dbo].[customers] ([Id], [Name]) VALUES (N'3', N'Mads')
GO

INSERT INTO [dbo].[customers] ([Id], [Name]) VALUES (N'4', N'William')
GO

INSERT INTO [dbo].[customers] ([Id], [Name]) VALUES (N'5', N'Test')
GO

SET IDENTITY_INSERT [dbo].[customers] OFF
GO


-- ----------------------------
-- Table structure for posts
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[posts]') AND type IN ('U'))
	DROP TABLE [dbo].[posts]
GO

CREATE TABLE [dbo].[posts] (
  [Id] int  IDENTITY(1,1) NOT NULL,
  [BankAccount] int DEFAULT ((0)) NOT NULL,
  [Value] decimal(30)  NOT NULL,
  [Date] datetime DEFAULT (getdate()) NOT NULL
)
GO

ALTER TABLE [dbo].[posts] SET (LOCK_ESCALATION = TABLE)
GO


-- ----------------------------
-- Records of posts
-- ----------------------------
SET IDENTITY_INSERT [dbo].[posts] ON
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'1', N'2', N'1000', N'2019-04-03 12:34:46.050')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'2', N'3', N'5000', N'2019-04-03 12:34:51.017')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'3', N'5', N'5000', N'2019-04-03 12:34:53.443')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'4', N'8', N'50000', N'2019-04-03 12:36:35.987')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'5', N'8', N'50000', N'2019-04-03 12:36:39.130')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'6', N'20', N'-100000', N'2019-04-03 12:37:08.860')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'7', N'28', N'100000', N'2019-04-03 12:37:08.860')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'8', N'20', N'-100000', N'2019-04-03 12:37:13.887')
GO

INSERT INTO [dbo].[posts] ([Id], [BankAccount], [Value], [Date]) VALUES (N'9', N'28', N'100000', N'2019-04-03 12:37:13.887')
GO

SET IDENTITY_INSERT [dbo].[posts] OFF
GO


-- ----------------------------
-- Procedure structure for InsertMoney
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[InsertMoney]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[InsertMoney]
GO

CREATE PROCEDURE [dbo].[InsertMoney](
	@InsertAmount AS DECIMAL,
	@AccountId AS INT
)
AS
BEGIN
  -- Make sure the account is active
	IF EXISTS (SELECT 1 FROM accounts WHERE Id = @AccountId AND Deleted IS NOT NULL)
	BEGIN
		RAISERROR('Target bank account is deleted', 16, 1)
	END
	ELSE
	BEGIN--/
	  -- Using ABS to ensure a positive insert value
		INSERT INTO posts (BankAccount, Value) VALUES (@AccountId, ABS(@InsertAmount))
	END
END
GO


-- ----------------------------
-- Procedure structure for WithdrawMoney
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[WithdrawMoney]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[WithdrawMoney]
GO

CREATE PROCEDURE [dbo].[WithdrawMoney](
	@WithdrawAmount AS DECIMAL,
	@AccountId AS INT,
	@Remaining AS DECIMAL OUTPUT
)
AS
BEGIN
  -- If check is not really nessecary as the Foreign Key constraint
	-- will fail if the account doesn't exist
	IF EXISTS (SELECT 1 FROM accounts WHERE Id = @AccountId AND Deleted IS NOT NULL)
	BEGIN
		RAISERROR('Target bank account is deleted', 16, 1)
	END
	ELSE
	BEGIN
		SET @Remaining = (SELECT Balance FROM accounts WHERE Id = @AccountId);
		SET @Remaining = @Remaining - @WithdrawAmount
		IF (@Remaining < 0)
		BEGIN
			RAISERROR('Insufficient funds', 16, 1)
		END
		ELSE
			BEGIN
			-- Using ABS to ensure a positive insert value then making it negative
			INSERT INTO posts (BankAccount, Value) VALUES (@AccountId, -ABS(@WithdrawAmount))
		END
	END
END
GO


-- ----------------------------
-- Procedure structure for TransferMoney
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[TransferMoney]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[TransferMoney]
GO

CREATE PROCEDURE [dbo].[TransferMoney](
	@TransferAmount AS DECIMAL,
	@SenderAccount AS INT,
	@ReceiverAccount AS INT
)
AS
BEGIN
	
	IF (@SenderAccount = @ReceiverAccount)
	BEGIN
		RAISERROR('Sender and Receiver are identical', 16, 1)
		RETURN
	END
	
	DECLARE @Remaining DECIMAL
	
	-- Execute Withdraw money procedure as it has the safety checks needed
	EXEC WithdrawMoney @TransferAmount, @SenderAccount, @Remaining OUTPUT
	IF (@Remaining >= 0) -- Withdraw was a success
	BEGIN
		EXEC InsertMoney @TransferAmount, @ReceiverAccount
		PRINT 'Transfer was succesful'
	END
	
END
GO


-- ----------------------------
-- Procedure structure for DeleteAccount
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[DeleteAccount]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[DeleteAccount]
GO

CREATE PROCEDURE [dbo].[DeleteAccount](
	@Account AS INT)
AS
BEGIN
	DECLARE @Result AS INT
	
	UPDATE accounts SET Deleted = GETDATE() WHERE Id = @Account AND Deleted IS NULL;
	SET @Result = @@ROWCOUNT
	
	IF (@Result = 0)
	BEGIN
		RAISERROR('Account is already deleted', 16, 1)
	END
END
GO


-- ----------------------------
-- Procedure structure for CreateCustomer
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[CreateCustomer]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[CreateCustomer]
GO

CREATE PROCEDURE [dbo].[CreateCustomer]
(
	@AccountHolder AS VARCHAR(100)
)
AS
BEGIN
	INSERT INTO customers (Name) VALUES (@AccountHolder)
	SELECT Scope_Identity() AS CustomerId
END
GO


-- ----------------------------
-- Procedure structure for CreateBankAccount
-- ----------------------------
IF EXISTS (SELECT * FROM sys.all_objects WHERE object_id = OBJECT_ID(N'[dbo].[CreateBankAccount]') AND type IN ('P', 'PC', 'RF', 'X'))
	DROP PROCEDURE[dbo].[CreateBankAccount]
GO

CREATE PROCEDURE [dbo].[CreateBankAccount](
	@CustomerId AS INT,
	@Startbalance AS DECIMAL
)
AS
BEGIN
	-- Foreign Key Constraint will ensure customer exists
	INSERT INTO accounts (Balance, Owner) VALUES (@StartBalance, @CustomerId)
END
GO


-- ----------------------------
-- Primary Key structure for table accounts
-- ----------------------------
ALTER TABLE [dbo].[accounts] ADD CONSTRAINT [PK__accounts__3214EC07086F585C] PRIMARY KEY CLUSTERED ([Id])
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)  
ON [PRIMARY]
GO


-- ----------------------------
-- Primary Key structure for table customers
-- ----------------------------
ALTER TABLE [dbo].[customers] ADD CONSTRAINT [PK__customer__3214EC07CF5DF440] PRIMARY KEY CLUSTERED ([Id])
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)  
ON [PRIMARY]
GO


-- ----------------------------
-- Triggers structure for table posts
-- ----------------------------
CREATE TRIGGER [dbo].[OnPostInsert]
ON [dbo].[posts]
WITH EXECUTE AS CALLER
FOR INSERT
AS
BEGIN
	SET NOCOUNT ON;

	DECLARE @PostValue DECIMAL
	DECLARE @AccountId INT
	
	SELECT @PostValue = INSERTED.Value       
       FROM INSERTED
	
	SELECT @AccountId = INSERTED.BankAccount       
       FROM INSERTED
	
	UPDATE accounts
  SET Balance = Balance + @PostValue,
		 Posts = Posts + 1
  WHERE Id = @AccountId
END
GO

CREATE TRIGGER [dbo].[PreventPostDeletion]
ON [dbo].[posts]
WITH EXECUTE AS CALLER
INSTEAD OF DELETE
AS
BEGIN
	RAISERROR('Deletion not supported on this table',16,1)
END
GO


-- ----------------------------
-- Primary Key structure for table posts
-- ----------------------------
ALTER TABLE [dbo].[posts] ADD CONSTRAINT [PK__posts__3214EC075BDCAE23] PRIMARY KEY CLUSTERED ([Id])
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)  
ON [PRIMARY]
GO


-- ----------------------------
-- Foreign Keys structure for table accounts
-- ----------------------------
ALTER TABLE [dbo].[accounts] ADD CONSTRAINT [FK_ACCOUNT_OWNER_ID] FOREIGN KEY ([Owner]) REFERENCES [dbo].[customers] ([Id]) ON DELETE NO ACTION ON UPDATE NO ACTION
GO


-- ----------------------------
-- Foreign Keys structure for table posts
-- ----------------------------
ALTER TABLE [dbo].[posts] ADD CONSTRAINT [FK_POSTS_ACCOUNT_ID] FOREIGN KEY ([BankAccount]) REFERENCES [dbo].[accounts] ([Id]) ON DELETE NO ACTION ON UPDATE NO ACTION
GO

