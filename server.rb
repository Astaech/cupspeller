#made by astaech

require 'socket'
require_relative 'wordlist.rb'

module Cupspeller
  class Server
    attr_reader :server, :wordlist

    def initialize
      puts 'server created'
      @server = TCPServer.open 50_000
      @wordlist = WordList.new
    end

    def accept
      unless @server.is_a? TCPServer
        raise ArgumentError, '!>server socket is not a TCPServer'
      end
      loop {
        Thread.start @server.accept do |client|
          puts 'thread created'
          client.puts '::bienvenue'
          self.game client
          client.close
        end
      }
    end

    def game client
      puts 'game called by thread'
      time = Time.new
      resp = ''
      @wordlist.word_list.each do |word|
        until resp == word do
          client.puts "::#{word}"
          resp = client.gets[0..-2]
        end
      end
      client.puts "::bravo: #{Time.new - time}s"
    end
  end
end

Cupspeller::Server.new.accept
