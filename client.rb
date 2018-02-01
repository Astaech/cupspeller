#made by astaech

require 'socket'

client = TCPSocket.new '127.0.0.1', 50_000

def func_write client
  puts "thread created, client is #{client}"
  loop {
    client.puts gets
  }
end

#Thread.start write_thread(client)

id_fork = fork { func_write client }
Process.detach id_fork

puts 'while seen'
while msg = client.gets do
  puts msg
end

client.close
