import os
import subprocess
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, MessageHandler, filters, CallbackContext

# 🔹 Replace with your actual Telegram bot token
TOKEN = "8082249856:AAEOnBF0kjELTRSdMzaeIIoFaemtNxnww7s"

# 🔹 Directory to save voice messages
SAVE_DIR = "voice_messages"
os.makedirs(SAVE_DIR, exist_ok=True)

async def start(update: Update, context: CallbackContext):
    """Handles the /start command"""
    await update.message.reply_text("Send me a voice message, and I'll save it as an MP3!")

async def handle_voice(update: Update, context: CallbackContext):
    """Handles voice messages, downloads, and converts to MP3"""
    if not update.message or not update.message.voice:
        return

    voice = update.message.voice
    file_id = voice.file_id

    # 🔹 Get the file from Telegram's servers
    new_file = await context.bot.get_file(file_id)

    # 🔹 Define file paths
    ogg_path = os.path.join(SAVE_DIR, f"{file_id}.ogg")
    mp3_path = os.path.join(SAVE_DIR, f"{file_id}.mp3")

    # 🔹 Download the voice message
    await new_file.download_to_drive(ogg_path)

    # 🔹 Convert to MP3 using ffmpeg
    try:
        subprocess.run(["ffmpeg", "-i", ogg_path, "-q:a", "2", mp3_path], check=True)
        await update.message.reply_text(f"Voice message saved as MP3: {mp3_path}")
        os.remove(ogg_path)  # Clean up
    except subprocess.CalledProcessError:
        await update.message.reply_text("Error converting the voice message to MP3.")

def main():
    """Starts the bot using ApplicationBuilder (recommended)"""
    app = ApplicationBuilder().token(TOKEN).build()

    app.add_handler(CommandHandler("start", start))
    app.add_handler(MessageHandler(filters.VOICE, handle_voice))

    print("Bot is running...")
    app.run_polling()

if __name__ == "__main__":
    main()
